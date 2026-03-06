/*-------------------------------------------------------------------------
 * solarPV.js  v2.0
 * PVWatts v8-based hourly solar PV production calculation in JavaScript.
 *
 * Ported from NREL SAM SSC source (BSD-3-Clause):
 *   lib_irradproc.cpp        – solar position (Michalsky/Iqbal algorithm),
 *                              surface/tracking angle calculations (incidence),
 *                              Perez (1990) sky diffuse model
 *   lib_pv_incidence_modifier.cpp – De Soto physical IAM (Snell+Bouguer)
 *   lib_cec6par.cpp          – NOCT steady-state cell temperature model
 *   cmod_pvwattsv8.cpp       – module/array parameters, Sandia inverter
 *
 * Module temperature coefficients updated to PVWatts v8 representative
 * modules (CS1H-320MS, SPR-E20-327, FS-6435A). NOCT per array type.
 *
 * Copyright Alliance for Sustainable Energy, LLC (BSD-3-Clause)
 *-------------------------------------------------------------------------*/
(function (root) {
  'use strict';

  var DTOR = Math.PI / 180.0;

  // ---- degree-based trig helpers -----------------------------------------
  function sind(d)  { return Math.sin(d * DTOR); }
  function cosd(d)  { return Math.cos(d * DTOR); }
  function acosd(x) { return Math.acos(Math.max(-1, Math.min(1, x))) / DTOR; }

  // =========================================================================
  // Module-type parameters (PVWatts v5)
  //   moduleType: 0=Standard, 1=Premium, 2=Thin Film
  //   gamma      : max-power temperature coefficient [fraction / °C]
  //   inoct_delta: adder to base NOCT (°C) for this module technology
  // =========================================================================
  // Module-type parameters (PVWatts v8)
  //   gamma   : max-power temperature coefficient [fraction / °C]
  //             Standard:  CS1H-320MS  (-0.37 %/°C)
  //             Premium:   SPR-E20-327 (-0.35 %/°C)
  //             Thin Film: FS-6435A    (-0.32 %/°C)
  //   eta_ref : STC power efficiency = Vmp*Imp / (1000 * Area) [fraction]
  var MODULE_TYPES = {
    0: { name: 'Standard',  gamma: -0.0037, eta_ref: 0.1912 },
    1: { name: 'Premium',   gamma: -0.0035, eta_ref: 0.2007 },
    2: { name: 'Thin Film', gamma: -0.0032, eta_ref: 0.1758 }
  };

  // =========================================================================
  // Array-type parameters (PVWatts v8)
  //   arrayType : 0=Fixed Open Rack, 1=Fixed Roof Mount, 2=1-Axis,
  //               3=1-Axis Backtracking, 4=2-Axis
  //   trackMode : 0=fixed, 1=1-axis, 2=2-axis
  //   tnoct     : installed NOCT (°C) — 49 for roof mount, 45 for all others
  //   gcr       : ground coverage ratio default (v8 default = 0.3)
  //   backtrack : enable backtracking?
  // =========================================================================
  var ARRAY_TYPES = {
    0: { name: 'Fixed Open Rack',     trackMode: 0, tnoct: 45, gcr: 0.3, backtrack: false },
    1: { name: 'Fixed Roof Mount',    trackMode: 0, tnoct: 49, gcr: 0.3, backtrack: false },
    2: { name: '1-Axis Tracking',     trackMode: 1, tnoct: 45, gcr: 0.3, backtrack: false },
    3: { name: '1-Axis Backtracking', trackMode: 1, tnoct: 45, gcr: 0.3, backtrack: true  },
    4: { name: '2-Axis Tracking',     trackMode: 2, tnoct: 45, gcr: 0.3, backtrack: false }
  };

  // =========================================================================
  // 1.  Solar position  (Michalsky 1988 / Iqbal azimuth correction)
  //     Ported from lib_irradproc.cpp :: solarpos()
  //
  //     Inputs  : year, month(1-12), day(1-31), hour(1-24), minute,
  //               lat(°N), lon(°E), tz(hours offset from UTC)
  //     Returns : { azm, zen, elv }  all in radians
  //               azm  : from North, clockwise (N=0, E=π/2, S=π, W=3π/2)
  //               zen  : zenith (0 = overhead, π/2 = horizon)
  //               elv  : elevation, with atmospheric refraction correction
  // =========================================================================
  function solarpos(year, month, day, hour, minute, lat, lon, tz) {
    var nday = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
    var k    = (year % 4 === 0) ? 1 : 0;      // leap-year day

    var jday = 0;
    for (var i = 0; i < month - 1; i++) { jday += nday[i]; }
    if (month > 2) jday += k + day; else jday += day;

    var zulu = hour + minute / 60.0 - tz;
    if (zulu <  0.0) { zulu += 24.0; jday -= 1; }
    if (zulu > 24.0) { zulu -= 24.0; jday += 1; }

    var delta = year - 1949;
    var leap  = Math.floor(delta / 4);
    var jd    = 32916.5 + delta * 365 + leap + jday + zulu / 24.0;
    var time  = jd - 51545.0;       // fractional days from J2000.0

    var mnlong = ((280.46    + 0.9856474 * time) % 360 + 360) % 360;
    var mnanom = ((357.528   + 0.9856003 * time) % 360 + 360) % 360 * DTOR;
    var eclong = ((mnlong + 1.915 * Math.sin(mnanom) + 0.020 * Math.sin(2 * mnanom)) % 360 + 360) % 360 * DTOR;
    var oblqec = (23.439 - 4e-7 * time) * DTOR;

    var num = Math.cos(oblqec) * Math.sin(eclong);
    var den = Math.cos(eclong);
    var ra  = Math.atan(num / den);
    if (den < 0)         ra += Math.PI;
    else if (num < 0)    ra += 2 * Math.PI;

    var dec  = Math.asin(Math.sin(oblqec) * Math.sin(eclong));
    var gmst = ((6.697375 + 0.0657098242 * time + zulu) % 24 + 24) % 24;
    var lmst = ((gmst + lon / 15.0) % 24 + 24) % 24 * 15 * DTOR;

    var ha = lmst - ra;
    if (ha < -Math.PI) ha += 2 * Math.PI;
    if (ha >  Math.PI) ha -= 2 * Math.PI;

    var lat_r = lat * DTOR;
    var arg   = Math.sin(dec) * Math.sin(lat_r) + Math.cos(dec) * Math.cos(lat_r) * Math.cos(ha);
    var elv   = Math.asin(Math.max(-1, Math.min(1, arg)));

    // Azimuth  (Iqbal formulation, handles southern latitudes correctly)
    var azm;
    if (Math.abs(Math.cos(elv)) < 1e-10) {
      azm = Math.PI;
    } else {
      arg = (Math.sin(elv) * Math.sin(lat_r) - Math.sin(dec)) / (Math.cos(elv) * Math.cos(lat_r));
      azm = Math.acos(Math.max(-1, Math.min(1, arg)));
      if ((ha <= 0 && ha >= -Math.PI) || ha >= Math.PI) azm = Math.PI - azm;
      else                                               azm = Math.PI + azm;
    }

    // Atmospheric refraction correction
    var elv_d = elv / DTOR;
    var refrac;
    if (elv_d > -0.56) {
      refrac = 3.51561 * (0.1594 + 0.0196 * elv_d + 2e-5 * elv_d * elv_d) /
                         (1.0    + 0.505  * elv_d  + 0.0845 * elv_d * elv_d);
    } else {
      refrac = 0.56;
    }
    if (elv_d + refrac > 90)  elv = Math.PI / 2;
    else                      elv = (elv_d + refrac) * DTOR;

    return { azm: azm, zen: Math.PI / 2 - elv, elv: elv };
  }

  // =========================================================================
  // 2.  True-tracking rotation for 1-axis trackers  (SAM truetrack)
  //     All inputs / output in degrees.
  //     Returns tracker rotation angle (degrees) relative to horizontal.
  //     axis_azimuth : 0 = N-S axis (panel sweeps E–W)
  //     axis_tilt    : 0 = horizontal axis
  // =========================================================================
  function truetrack_deg(solar_azimuth, solar_zenith, axis_tilt, axis_azimuth) {
    var solar_elv = 90 - solar_zenith;
    var sx  = cosd(solar_elv) * sind(solar_azimuth);
    var sy  = cosd(solar_elv) * cosd(solar_azimuth);
    var sz  = sind(solar_elv);
    var sinYa = sind(axis_azimuth), cosYa = cosd(axis_azimuth);
    var sinBa = sind(axis_tilt),    cosBa = cosd(axis_tilt);
    var sxp = sx * cosYa - sy * sinYa;
    var szp = sx * sinYa * sinBa + sy * sinBa * cosYa + sz * cosBa;
    return Math.atan2(sxp, szp) / DTOR;
  }

  // =========================================================================
  // 3.  Backtracking rotation  (closed-form, pvlib / SAM convention)
  //     All inputs / output in degrees.
  //     Adjusts tracker angle to prevent row-to-row shading.
  // =========================================================================
  function backtrack_deg(tt_rot, gcr) {
    var cp   = Math.abs(cosd(tt_rot)) / gcr;
    if (cp >= 1) return tt_rot;
    var sign = tt_rot >= 0 ? 1 : -1;
    return tt_rot + (-sign) * acosd(cp);
  }

  // =========================================================================
  // 4.  Incidence-angle and surface-orientation calculator
  //     Ported from lib_irradproc.cpp :: incidence()
  //
  //     mode    : 0=fixed, 1=1-axis, 2=2-axis, 3=azimuth-axis
  //     tilt_deg, sazm_deg  : fixed surface tilt / azimuth (degrees)
  //     rlim_deg : tracker rotation limit ±(degrees)
  //     zen_rad, azm_rad    : solar zenith / azimuth (radians)
  //     en_backtrack, gcr  : backtracking settings
  //
  //     Returns { inc, tilt, sazm, rot, btdiff }  all in radians
  // =========================================================================
  function incidence(mode, tilt_deg, sazm_deg, rlim_deg, zen_rad, azm_rad, en_backtrack, gcr) {
    var rlim = rlim_deg * DTOR;
    var inc = 0, tilt, sazm, rot = 0, btdiff = 0, arg;

    if (mode === 0 || mode === 3) {
      /* ---- Fixed / Azimuth-axis ---- */
      tilt = tilt_deg * DTOR;
      sazm = (mode === 0) ? sazm_deg * DTOR : azm_rad;   // azimuth-axis tracks sun azimuth
      arg  = Math.sin(zen_rad) * Math.cos(azm_rad - sazm) * Math.sin(tilt) +
             Math.cos(zen_rad) * Math.cos(tilt);
      inc  = Math.acos(Math.max(-1, Math.min(1, arg)));

    } else if (mode === 1) {
      /* ---- 1-Axis Tracking ---- */
      var xtilt = tilt_deg * DTOR;
      var xsazm = sazm_deg * DTOR;

      rot         = truetrack_deg(azm_rad / DTOR, zen_rad / DTOR, tilt_deg, sazm_deg) * DTOR;
      var tt_rot  = rot;
      rot         = Math.max(-rlim, Math.min(rlim, rot));

      if (en_backtrack) {
        var bt_rot = backtrack_deg(tt_rot / DTOR, gcr) * DTOR;
        bt_rot  = Math.max(-rlim, Math.min(rlim, bt_rot));
        btdiff  = bt_rot - rot;
        rot     = bt_rot;
      }

      // Effective tilt from rotation
      arg  = Math.cos(xtilt) * Math.cos(rot);
      tilt = Math.acos(Math.max(-1, Math.min(1, arg)));

      // Effective surface azimuth from rotation
      if (tilt === 0) {
        sazm = Math.PI;
      } else {
        arg = Math.sin(rot) / Math.sin(tilt);
        arg = Math.max(-1, Math.min(1, arg));
        if      (rot < -Math.PI / 2) sazm = xsazm - Math.PI - Math.asin(arg);
        else if (rot >  Math.PI / 2) sazm = xsazm + Math.PI - Math.asin(arg);
        else                          sazm = Math.asin(arg)  + xsazm;
        sazm = ((sazm % (2 * Math.PI)) + 2 * Math.PI) % (2 * Math.PI);
      }

      arg = Math.sin(zen_rad) * Math.cos(azm_rad - sazm) * Math.sin(tilt) +
            Math.cos(zen_rad) * Math.cos(tilt);
      inc = Math.acos(Math.max(-1, Math.min(1, arg)));

    } else if (mode === 2) {
      /* ---- 2-Axis Tracking (always faces sun) ---- */
      tilt = zen_rad;
      sazm = azm_rad;
      inc  = 0;
    }

    return { inc: inc, tilt: tilt, sazm: sazm, rot: rot, btdiff: btdiff };
  }

  // =========================================================================
  // 5.  Physical transmittance  (Snell's law + Bouguer's law)
  //     Ported from lib_pv_incidence_modifier.cpp :: transmittance()
  //     Supporting function for the De Soto incidence-angle modifier.
  // =========================================================================
  // Optical constants for AR-coated glass module cover (PVWatts v8 defaults)
  var N_ARC   = 1.29;     // refractive index of anti-reflective coating
  var N_GLASS = 1.586;    // refractive index of glass
  var K_ARC   = 4.0;      // extinction coefficient of AR coating (1/m)
  var L_ARC   = 1.0e-4;   // thickness of AR coating (m)
  var K_GLASS = 4.0;      // extinction coefficient of glass (1/m)
  var L_GLASS = 0.002;    // thickness of glass (m)
  var AOI_MIN = 1.0;      // minimum valid AOI (degrees)
  var AOI_MAX = 89.0;     // maximum valid AOI (degrees)

  function transmittance(theta1_deg, n_cover, n_incoming, k, l_thick) {
    var theta1 = theta1_deg * DTOR;
    var sinT2  = Math.max(-1, Math.min(1, n_incoming / n_cover * Math.sin(theta1)));
    var theta2 = Math.asin(sinT2);
    var refl;
    if (Math.abs(theta1 + theta2) < 1e-10 || Math.abs(theta1 - theta2) < 1e-10) {
      refl = 0.0;
    } else {
      var rs = Math.pow(Math.sin(theta2 - theta1), 2) / Math.pow(Math.sin(theta2 + theta1), 2);
      var rp = Math.pow(Math.tan(theta2 - theta1), 2) / Math.pow(Math.tan(theta2 + theta1), 2);
      refl   = (rs + rp) / 2.0;
    }
    var tau = (1.0 - refl) * Math.exp(-k * l_thick / Math.cos(theta2));
    return { tau: tau, theta2_deg: theta2 / DTOR };
  }

  // =========================================================================
  // 6.  De Soto incidence-angle modifier
  //     Ported from lib_pv_incidence_modifier.cpp ::
  //       calculateIrradianceThroughCoverDeSoto()
  //     Replaces PVWatts v5 King polynomial (transpoa).
  //     All v8 module types have AR-coated glass (ar_glass = true).
  //
  //     inc_deg  : angle of incidence on surface (degrees)
  //     tilt_deg : surface tilt (degrees)
  //     G_beam   : POA beam irradiance (W/m²)
  //     G_sky    : POA sky-diffuse irradiance (W/m²)
  //     G_gnd    : POA ground-reflected irradiance (W/m²)
  //     Returns effective transmitted POA irradiance (W/m²)
  // =========================================================================
  function tpoaDesoto(inc_deg, tilt_deg, G_beam, G_sky, G_gnd) {
    var theta = Math.max(AOI_MIN, Math.min(AOI_MAX, inc_deg));
    var tilt  = Math.max(AOI_MIN, Math.min(AOI_MAX, tilt_deg));

    // Transmittance at near-normal (1°) through AR coating then glass
    var tn_arc   = transmittance(1.0, N_ARC, 1.0, K_ARC, L_ARC);
    var tn_glass = transmittance(tn_arc.theta2_deg, N_GLASS, N_ARC, K_GLASS, L_GLASS);
    var tau_norm = tn_arc.tau * tn_glass.tau;

    // Beam transmittance at actual incidence angle
    var tb_arc   = transmittance(theta, N_ARC, 1.0, K_ARC, L_ARC);
    var tb_glass = transmittance(tb_arc.theta2_deg, N_GLASS, N_ARC, K_GLASS, L_GLASS);
    var tau_beam = tb_arc.tau * tb_glass.tau;

    // Sky diffuse: equivalent angle from Duffie & Beckman Eqn 5.4.2
    var theta_sky = 59.7 - 0.1388 * tilt + 0.001497 * tilt * tilt;
    var tau_sky   = transmittance(theta_sky, N_GLASS, 1.0, K_GLASS, L_GLASS).tau;

    // Ground diffuse: equivalent angle from Duffie & Beckman Eqn 5.4.1
    var theta_gnd = 90.0 - 0.5788 * tilt + 0.002693 * tilt * tilt;
    var tau_gnd   = transmittance(theta_gnd, N_GLASS, 1.0, K_GLASS, L_GLASS).tau;

    // Component IAM factors (Kta), capped at 1.0
    var Kta_beam = Math.min(1.0, tau_beam / tau_norm);
    var Kta_sky  = Math.min(1.0, tau_sky  / tau_norm);
    var Kta_gnd  = Math.min(1.0, tau_gnd  / tau_norm);

    return Math.max(0, G_beam * Kta_beam + G_sky * Kta_sky + G_gnd * Kta_gnd);
  }

  // =========================================================================
  // 7.  NOCT steady-state cell temperature  (noct_celltemp_t)
  //     Ported from lib_cec6par.cpp :: noct_celltemp_t::operator()
  //     Replaces PVWatts v5 dynamic pvwatts_celltemp.
  //
  //     tpoa    : IAM-corrected POA irradiance (W/m²)
  //     wspd    : wind speed (m/s)
  //     tamb    : ambient dry-bulb temperature (°C)
  //     tnoct   : installed NOCT for this array type (°C) — 45 or 49
  //     eta_ref : module STC power efficiency (fraction)
  //     Returns cell temperature (°C)
  // =========================================================================
  var TAU_ALPHA = 0.9;    // product of cover transmittance × cell absorptance
  var I_NOCT    = 800.0;  // irradiance at NOCT test conditions (W/m²)
  var TAMB_NOCT = 20.0;   // ambient temperature at NOCT test conditions (°C)

  function noctCelltemp(tpoa, wspd, tamb, tnoct, eta_ref) {
    if (tpoa <= 0) return tamb;
    var w = Math.max(0.001, wspd);
    return tamb + (tpoa / I_NOCT) * (tnoct - TAMB_NOCT) *
           (1.0 - eta_ref / TAU_ALPHA) * 9.5 / (5.7 + 3.8 * w);
  }

  // =========================================================================
  // 8.  Sandia inverter model  (simplified, C0=C1=C2=C3=0, Pntare=0)
  //     Ported from cmod_pvwattsv8.cpp inverter block.
  //     Replaces PVWatts v5 dctoac() efficiency-curve function.
  //
  //     dc   : DC power input (W)
  //     Paco : AC nameplate power (W)  = dc_nameplate / dc_ac_ratio
  //     Pdco : DC input for rated AC  = Paco / inv_eff
  //     Pso  : self-consumption threshold (W) — size-dependent in v8
  //     Returns AC power (W), clipped at Paco, floored at 0.
  // =========================================================================
  function sandiaInverter(dc, Paco, Pdco, Pso) {
    if (dc <= Pso) return 0.0;
    var ac = Paco / (Pdco - Pso) * (dc - Pso);
    if (ac > Paco) ac = Paco;
    if (ac < 0.0)  ac = 0.0;
    return ac;
  }

  // =========================================================================
  // 9.  Main annual PV production  –  computeAnnualPV(epw, params)
  //
  //     params = {
  //       dcCapacity_kW : DC nameplate capacity (kW)
  //       moduleType    : 0=Standard, 1=Premium, 2=Thin Film
  //       arrayType     : 0=Fixed Open Rack, 1=Fixed Roof Mount, 2=1-Axis,
  //                       3=1-Axis Backtracking, 4=2-Axis
  //       systemLosses  : fraction (e.g. 0.14 for 14 %)
  //       tilt_deg      : panel tilt in degrees (0=horizontal; ignored for 2-axis)
  //       azimuth_deg   : panel azimuth in degrees
  //                       (N=0, E=90, S=180, W=270)
  //     }
  //
  //     Returns { monthly: Array[12] (kWh), annual: Number (kWh) }
  //
  //     Calculation notes (PVWatts v8):
  //       • Solar position at mid-hour (EPW hour N → local time N–0.5 h)
  //       • Perez (1990) sky diffuse model
  //       • De Soto physical incidence-angle modifier (AR glass)
  //       • NOCT steady-state cell temperature model
  //       • Simplified linear DC model with v8 temperature coefficients
  //       • Sandia inverter model (C0=C1=C2=C3=0, size-dependent Pso)
  //       • DC:AC ratio = 1.1,  inverter efficiency = 0.96
  // =========================================================================
  function computeAnnualPV(epw, params) {
    var DC_AC_RATIO = 1.1;
    var INV_EFF     = 0.96;   // fraction
    var ALBEDO      = 0.2;
    var RLIM_DEG    = 45;
    var IREF        = 1000.0; // W/m² at STC
    var REFTEM      = 25.0;   // °C at STC

    var mpar   = MODULE_TYPES[params.moduleType] || MODULE_TYPES[0];
    var apar   = ARRAY_TYPES[params.arrayType]   || ARRAY_TYPES[1];

    var gamma   = mpar.gamma;                     // temp coefficient [/°C]
    var tnoct   = apar.tnoct;                     // NOCT [°C]
    var eta_ref = mpar.eta_ref;                   // STC efficiency
    var refpwr  = params.dcCapacity_kW * 1000;    // W (DC nameplate)
    var f_loss  = 1.0 - params.systemLosses;      // DC loss factor

    // Sandia inverter parameters
    var Paco = refpwr / DC_AC_RATIO;              // AC nameplate (W)
    var Pdco = Paco / INV_EFF;                    // DC input for rated AC (W)
    var Pso;  // self-consumption threshold (size-dependent per v8)
    if      (refpwr < 10000)   Pso = 0.002246 * Paco; // residential
    else if (refpwr < 1000000) Pso = 0.002478 * Paco; // commercial
    else                       Pso = 0.004931 * Paco; // utility

    var lat = parseFloat(epw.latitude);
    var lon = parseFloat(epw.longitude);
    var tz  = parseFloat(epw.timeZone);

    // Perez (1990) sky-diffuse model coefficients (8 clearness bins)
    var F11R = [-0.0083117, 0.1299457, 0.3296958, 0.5682053,
                 0.8730280, 1.1326077, 1.0601591, 0.6777470];
    var F12R = [ 0.5877285, 0.6825954, 0.4868735, 0.1874525,
                -0.3920403,-1.2367284,-1.5999137,-0.3272588];
    var F13R = [-0.0620636,-0.1513752,-0.2210958,-0.2951290,
                -0.3616149,-0.4118494,-0.3589221,-0.2504286];
    var F21R = [-0.0596012,-0.0189325, 0.0554140, 0.1088631,
                 0.2255647, 0.2877813, 0.2642124, 0.1561313];
    var F22R = [ 0.0721249, 0.0659650,-0.0639588,-0.1519229,
                -0.4620442,-0.8230357,-1.1272340,-1.3765031];
    var F23R = [-0.0220216,-0.0288748,-0.0260542,-0.0139754,
                 0.0012448, 0.0558651, 0.1310694, 0.2506212];
    var EPSBINS = [1.065, 1.23, 1.5, 1.95, 2.8, 4.5, 6.2];
    var B2 = 5.534e-6; // Perez clearness correction constant

    // Pull EPW arrays once
    var months = epw.month();
    var days   = epw.day();
    var hours  = epw.hour();
    var Years  = epw.year();
    var DNI    = epw.directNormalRadiation();
    var DHI    = epw.diffuseHorizontalRadiation();
    var Tamb   = epw.dryBulbTemperature();
    var Wspd   = epw.windSpeed();

    var monthlyAC  = new Array(12);
    var monthlyPOA = new Array(12);
    for (var k = 0; k < 12; k++) { monthlyAC[k] = 0; monthlyPOA[k] = 0; }

    var n = epw.weatherData.length;
    var hourlyAC = new Array(n);
    for (var k = 0; k < n; k++) { hourlyAC[k] = 0; }

    for (var i = 0; i < n; i++) {
      var mo   = months[i];
      var day  = days[i];
      var hr   = hours[i];    // EPW 1–24
      var yr   = Years[i];
      var beam = Math.max(0, DNI[i]);
      var diff = Math.max(0, DHI[i]);
      var tamb = Tamb[i];
      var wspd = Math.max(0, Wspd[i]);

      // Solar position at mid-hour
      var sol    = solarpos(yr, mo, day, hr - 1, 30, lat, lon, tz);
      var zen    = sol.zen;
      var solazi = sol.azm;

      // Skip hours when sun is below horizon
      if (zen >= Math.PI / 2) continue;

      // Surface orientation and angle of incidence
      var ang    = incidence(apar.trackMode, params.tilt_deg, params.azimuth_deg,
                             RLIM_DEG, zen, solazi, apar.backtrack, apar.gcr);
      var inc_rad = ang.inc;
      var tilt_r  = ang.tilt;
      var tilt_d  = tilt_r / DTOR;   // tilt in degrees (for De Soto IAM)
      var inc_d   = inc_rad / DTOR;  // AOI in degrees

      // ---- Perez (1990) sky-diffuse model --------------------------------
      var beam_poa = beam * Math.max(0, Math.cos(inc_rad));
      var sky_diff, gnd_diff;
      var zen_d = zen / DTOR;

      if (zen_d > 87.5) {
        // Near-horizon: treat diffuse as isotropic
        sky_diff = diff * (1.0 + Math.cos(tilt_r)) / 2.0;
      } else if (diff <= 0) {
        sky_diff = 0.0;
      } else {
        var CZ      = Math.cos(zen);
        var ZH      = Math.max(CZ, Math.cos(85.0 * DTOR)); // floor at 85°
        var AIRMASS = 1.0 / (CZ + 0.15 * Math.pow(93.9 - zen_d, -1.253));
        var DELTA   = diff * AIRMASS / 1367.0;
        var T       = Math.pow(zen_d, 3);
        var EPS     = (beam + diff) / diff;
        EPS = (EPS + T * B2) / (1.0 + T * B2);
        var bin = 0;
        while (bin < 7 && EPS > EPSBINS[bin]) bin++;
        var xF1 = F11R[bin] + F12R[bin] * DELTA + F13R[bin] * zen;
        var F1  = Math.max(0, xF1);
        var F2  = F21R[bin] + F22R[bin] * DELTA + F23R[bin] * zen;
        var ZC  = Math.max(0, Math.cos(inc_rad));
        var A   = diff * (1.0 - F1) * (1.0 + Math.cos(tilt_r)) / 2.0; // isotropic
        var B   = diff * F1 * ZC / ZH;                                  // circumsolar
        var C   = diff * F2 * Math.sin(tilt_r);                         // horizon
        sky_diff = Math.max(0, A + B + C);
      }

      // Ground-reflected diffuse (Perez formulation uses beam*cos(zen)+diff for GHI)
      gnd_diff = Math.max(0, ALBEDO * (beam * Math.cos(zen) + diff) *
                          (1.0 - Math.cos(tilt_r)) / 2.0);

      // Incident (pre-IAM) plane-of-array irradiance (W/m²)
      var poa_incident = beam_poa + sky_diff + gnd_diff;

      // ---- De Soto incidence-angle modifier (AR glass) -------------------
      var tpoa = tpoaDesoto(inc_d, tilt_d, beam_poa, sky_diff, gnd_diff);

      // ---- NOCT steady-state cell temperature ----------------------------
      var Tcell = noctCelltemp(tpoa, wspd, tamb, tnoct, eta_ref);

      // ---- DC power (linear model, v8 temperature coefficients) ----------
      var dc = 0;
      if (tpoa > 125) {
        dc = refpwr * (1.0 + gamma * (Tcell - REFTEM)) * tpoa / IREF * f_loss;
      } else if (tpoa > 0.1) {
        dc = refpwr * (1.0 + gamma * (Tcell - REFTEM)) * 0.008 * tpoa * tpoa / IREF * f_loss;
      }
      if (dc < 0) dc = 0;

      // ---- Sandia inverter -----------------------------------------------
      var ac = sandiaInverter(dc, Paco, Pdco, Pso);
      hourlyAC[i] = ac;                           // W, instantaneous
      monthlyAC[mo - 1]  += ac / 1000;           // W → kWh (1-hour timestep)
      monthlyPOA[mo - 1] += poa_incident / 1000; // W/m² → kWh/m²
    }

    var annual = 0, annualPOA = 0;
    for (var m = 0; m < 12; m++) { annual += monthlyAC[m]; annualPOA += monthlyPOA[m]; }

    return { monthly: monthlyAC, annual: annual, monthlyPOA: monthlyPOA, annualPOA: annualPOA, hourlyAC: hourlyAC };
  }

  // ---- Public API --------------------------------------------------------
  var SolarPV = {
    computeAnnualPV : computeAnnualPV,
    MODULE_TYPES    : MODULE_TYPES,
    ARRAY_TYPES     : ARRAY_TYPES
  };

  // UMD-style export
  if (typeof module !== 'undefined' && module.exports) {
    module.exports = SolarPV;
  } else {
    root.SolarPV = SolarPV;
  }

}(typeof window !== 'undefined' ? window : this));
