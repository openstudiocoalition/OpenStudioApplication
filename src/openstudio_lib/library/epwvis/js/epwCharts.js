/*-------------------------------------------------------------------------
 * epwCharts.js
 * Chart library for epwvis
 *
 * DEPENDENCIES
 *  - d3.js
 *-------------------------------------------------------------------------*/

// ---- User-configurable appearance (persisted across redraws) ----
window._plotColor  = window._plotColor  || 'steelblue';
window._hmScheme   = window._hmScheme   || 'rainbow';

var _hmSchemes = {
    'rainbow': ['darkblue','blue','cyan','greenyellow','yellow','orange','red','darkred'],
    'rd-bu':   ['#053061','#2166ac','#4393c3','#92c5de','#f7f7f7','#f4a582','#d6604d','#b2182b'],
    'viridis': ['#440154','#414487','#2a788e','#22a884','#7ad151','#fde725'],
    'inferno': ['#000004','#3f0f70','#8c2981','#dd513b','#fba40a','#fcffa4'],
    'blues':   ['#f7fbff','#c6dbef','#6baed6','#2171b5','#08306b']
};
function getHmColors() {
    return _hmSchemes[window._hmScheme] || _hmSchemes['rainbow'];
}

function clearEPWCharts() {
    d3.selectAll("svg").remove();
    window.updatePsychroPoints    = null;
    window.updateTimeseries        = null;
    window.updateWindRose          = null;
    window.updateHeatmaps          = null;
    window.updateTabular           = null;
    window.updateScatter           = null;
    window._scatterLastActiveSet   = null;
    window.updateDurationCurve     = null;
    window._durCurrentVarId        = null;
    window._durLastActive          = null;
    window.updateDegreeDays        = null;
    window._ddLastActive           = null;
    window._ddUnitSystem           = null;
    window._pvLastResult           = null;
    window._pvLastEpw              = null;
    window._pvHourlyAC             = null;
    window._tsAllData              = null;
    window._tsLastActive       = null;
    window._tsCurrentVarId     = null;
    window._hmColorScales      = null;
    window._hmLastActiveSet    = null;
    window._tblLastCols        = null;
    window._tblLastRows        = null;
    window._tblLastVarDef      = null;
    if (window._hmDocClickHandler) {
        document.removeEventListener('click', window._hmDocClickHandler);
        window._hmDocClickHandler = null;
    }
    if (window._vfDocClickHandler) {
        document.removeEventListener('click', window._vfDocClickHandler);
        window._vfDocClickHandler = null;
    }
    var hmContainer = document.getElementById('heatmapChartsContainer');
    if (hmContainer) { hmContainer.innerHTML = ''; }
};

function epwData(epw,value){
    // for constructing a data object with just one value
    var month = epw.month();
    var day = epw.day();
    var hour = epw.hour();
    var dayOfYear = [];
    var data = [];

    for (var i=0; i < value.length; i++){
      dayOfYear[i] = Math.floor(i/24)+1;
      datum = {"index":i,"month":month[i],"day":day[i],"hour":hour[i],"dayOfYear":dayOfYear[i],"value":value[i]};
      data.push(datum);
    };

    //console.log(data);
    return data;
};

function epwDataDefault(epw,unitSystem){
    var month = epw.month(),
        day = epw.day(),
        hour = epw.hour(),
        dryBulbTemperature = epw.dryBulbTemperature(),
        relativeHumidity = epw.relativeHumidity(),
        dewPointTemperature = epw.dewPointTemperature(),
        wetBulbTemperature = epw.wetBulbTemperature(),
        humidityRatio = epw.humidityRatio(),
        specificHumidity = epw.specificHumidity(),
        vaporPressure = epw.vaporPressure(),
        moistAirEnthalpy = epw.moistAirEnthalpy(),
        moistAirVolume = epw.moistAirVolume(),
        degreeOfSaturation = epw.degreeOfSaturation(),
        moistAirDensity = epw.moistAirDensity(),
        windSpeed = epw.windSpeed(),
        windDirection = epw.windDirection(),
        totalSkyCover = epw.totalSkyCover(),
        globalHorizontalRadiation = epw.globalHorizontalRadiation(),
        directNormalRadiation = epw.directNormalRadiation(),
        diffuseHorizontalRadiation = epw.diffuseHorizontalRadiation(),
        atmosphericStationPressure = epw.atmosphericStationPressure(),
        opaqueSkyCover = epw.opaqueSkyCover(),
        visibility = epw.visibility().map(function(v) { return (+v >= 9999) ? null : +v; }),
        ceilingHeight = epw.ceilingHeight().map(function(v) { return (+v >= 99999) ? null : +v; }),
        precipitableWater = epw.precipitableWater().map(function(v) { return (+v >= 999) ? null : +v; }),
        aerosolOpticalDepth = epw.aerosolOpticalDepth().map(function(v) { return (+v >= 0.999) ? null : +v; }),
        snowDepth = epw.snowDepth().map(function(v) { return (+v >= 999) ? null : +v; }),
        daysSinceLastSnowfall = epw.daysSinceLastSnowfall().map(function(v) { return (+v >= 99) ? null : +v; }),
        albedo = epw.albedo().map(function(v) { return (+v >= 2) ? null : +v; }),
        liquidPrecipitationDepth = epw.liquidPrecipitationDepth().map(function(v) { return (+v >= 999) ? null : +v; }),
        liquidPrecipitationQuantity = epw.liquidPrecipitationQuantity().map(function(v) { return (+v >= 99) ? null : +v; }),
        dayOfYear = [],
        data = [];

        if (unitSystem == "IP") {
          dryBulbTemperature = convertCtoF(dryBulbTemperature);
          dewPointTemperature = convertCtoF(dewPointTemperature);
          windSpeed = convertKnots(windSpeed);
        };

    for (var i=0; i < month.length; i++){
      dayOfYear[i] = Math.floor(i/24)+1;
      datum = {"index":i,"month":month[i],"day":day[i],
               "hour":hour[i],"dayOfYear":dayOfYear[i],
               "dryBulbTemperature":dryBulbTemperature[i],
               "relativeHumidity":relativeHumidity[i],
               "dewPointTemperature":dewPointTemperature[i],
               "wetBulbTemperature":wetBulbTemperature[i],
               "humidityRatio":humidityRatio[i],
               "specificHumidity":specificHumidity[i],
               "vaporPressure":vaporPressure[i],
               "moistAirEnthalpy":moistAirEnthalpy[i],
               "moistAirVolume":moistAirVolume[i],
               "degreeOfSaturation":degreeOfSaturation[i],
               "moistAirDensity":moistAirDensity[i],
               "windSpeed":windSpeed[i],
               "windDirection":windDirection[i],
               "totalSkyCover":totalSkyCover[i],
               "globalHorizontalRadiation":globalHorizontalRadiation[i],
               "directNormalRadiation":directNormalRadiation[i],
               "diffuseHorizontalRadiation":diffuseHorizontalRadiation[i],
               "atmosphericStationPressure":atmosphericStationPressure[i],
               "opaqueSkyCover":opaqueSkyCover[i],
               "visibility":visibility[i],
               "ceilingHeight":ceilingHeight[i],
               "precipitableWater":precipitableWater[i],
               "aerosolOpticalDepth":aerosolOpticalDepth[i],
               "snowDepth":snowDepth[i],
               "daysSinceLastSnowfall":daysSinceLastSnowfall[i],
               "albedo":albedo[i],
               "liquidPrecipitationDepth":liquidPrecipitationDepth[i],
               "liquidPrecipitationQuantity":liquidPrecipitationQuantity[i],
               "pvAC_kWh":window._pvHourlyAC ? ((window._pvHourlyAC[i] || 0) / 1000) : 0};
      data.push(datum);
    };

    //console.log(data);
    return data;
};

//unit coversion functions, could be done more cleanly
function valCtoF(value,index,arr) {
    arr[index] = 32 + value*1.8;
};
function convertCtoF(array) {
    array.forEach(valCtoF);
    return array;
};
function valKnots(value,index,arr) {
    arr[index] = value*1.94384;
};
function convertKnots(array) {
    array.forEach(valKnots);
    return array;
};

//initialization code for the drybulb temperature heatmap
function epwTempHeatmap(epw) {
    params = {};
    var value = [];
    if (unitSystem == "IP") {
        value = convertCtoF(epw.dryBulbTemperature());
        params.unit = "\xB0F";
    } else {
        value = epw.dryBulbTemperature();
        params.unit = "\xB0C";
    };
    var data = epwData(epw,value); //encoding most of the object construction here
    params.id = "#epwTempHeatmap";
    params.min_value = Math.min.apply(Math,value);
    params.max_value = Math.max.apply(Math,value);
    params.steps = 7;
    params.step_colors = ['darkblue','blue', 'cyan', 'greenyellow', 'yellow', 'orange', 'red','darkred'];
    epwHeatmap(data,params);
};

//initialization code for the cloud cover heatmap
function epwCloudHeatmap(epw) {
    var value = epw.totalSkyCover();
    var data = epwData(epw,value); //encoding most of the object construction here
    params = {};
    params.id = "#epwCloudHeatmap";
    params.min_value = 0;
    params.max_value = 10;
    params.unit = "";
    params.steps = 10;
    params.step_colors = ['#6fdcfb','#6bcde9', '#68bfd8', '#65b1c7', '#62a3b6', '#5f95a5', '#5b8793','#587982','#556b71','#525d60','#4f4f4f'];
    epwHeatmap(data,params);
};

/*-------------------------------------------------------------------------
 * epwHeatmapCharts
 * Checkbox-dropdown-driven dynamic heatmap renderer.
 * Selections persist across unit-system re-renders via window._hmSelectedVars.
 *-------------------------------------------------------------------------*/
function epwHeatmapCharts(epw) {
    var isIP = (unitSystem === 'IP');
    var totalHours = epw.dryBulbTemperature().length;
    window._hmColorScales   = {};
    window._hmLastActiveSet = null;

    var hmVarDefs = [
        { id: 'dryBulbTemperature',
          label: 'Dry Bulb Temperature',       defaultChecked: true,
          getVal: function(e, ip) { return ip ? convertCtoF(e.dryBulbTemperature()) : e.dryBulbTemperature(); },
          unit: function(ip) { return ip ? '\u00B0F' : '\u00B0C'; },
          steps: 7,  colors: ['darkblue','blue','cyan','greenyellow','yellow','orange','red','darkred'],
          autoMinMax: true },
        { id: 'dewPointTemperature',
          label: 'Dew Point Temperature',      defaultChecked: false,
          getVal: function(e, ip) { return ip ? convertCtoF(e.dewPointTemperature()) : e.dewPointTemperature(); },
          unit: function(ip) { return ip ? '\u00B0F' : '\u00B0C'; },
          steps: 7,  colors: ['darkblue','blue','cyan','greenyellow','yellow','orange','red','darkred'],
          autoMinMax: true },
        { id: 'wetBulbTemperature',
          label: 'Wet Bulb Temperature',       defaultChecked: false,
          getVal: function(e, ip) {
              var v = e.wetBulbTemperature();
              return ip ? v.map(function(x) { return x === null ? null : 32 + x * 1.8; }) : v;
          },
          unit: function(ip) { return ip ? '\u00B0F' : '\u00B0C'; },
          steps: 7,  colors: ['darkblue','blue','cyan','greenyellow','yellow','orange','red','darkred'],
          autoMinMax: true },
        { id: 'relativeHumidity',
          label: 'Relative Humidity',          defaultChecked: false,
          getVal: function(e, ip) { return e.relativeHumidity(); },
          unit: function(ip) { return '%'; },
          steps: 10, colors: ['#f7fbff','#deebf7','#c6dbef','#9ecae1','#6baed6','#4292c6','#2171b5','#08519c','#08306b','#041a3a','#000d1a'],
          autoMinMax: false, fixMin: 0, fixMax: 100 },
        { id: 'humidityRatio',
          label: 'Humidity Ratio',             defaultChecked: false,
          getVal: function(e, ip) {
              var v = e.humidityRatio();
              return v.map(function(x) { return x === null ? null : (ip ? x * 7000 : x * 1000); });
          },
          unit: function(ip) { return ip ? 'gr/lb' : 'g/kg'; },
          steps: 7,  colors: ['#ffffcc','#d9f0a3','#addd8e','#78c679','#41ab5d','#238443','#006837','#004529'],
          autoMinMax: true },
        { id: 'enthalpy',
          label: 'Enthalpy',                   defaultChecked: false,
          getVal: function(e, ip) {
              var v = e.moistAirEnthalpy();
              return v.map(function(x) { return x === null ? null : (ip ? x * 4.29923e-4 : x * 0.001); });
          },
          unit: function(ip) { return ip ? 'BTU/lb' : 'kJ/kg'; },
          steps: 7,  colors: ['darkblue','blue','cyan','greenyellow','yellow','orange','red','darkred'],
          autoMinMax: true },
        { id: 'globalHorizontalRadiation',
          label: 'Global Horiz. Radiation',    defaultChecked: false,
          getVal: function(e, ip) { return e.globalHorizontalRadiation(); },
          unit: function(ip) { return 'Wh/m\u00B2'; },
          steps: 7,  colors: ['#000033','#003399','#0066ff','#66ccff','#ffff00','#ff9900','#ff3300','#ffffff'],
          autoMinMax: false, fixMin: 0, fixMax: null },
        { id: 'directNormalRadiation',
          label: 'Direct Normal Radiation',    defaultChecked: false,
          getVal: function(e, ip) { return e.directNormalRadiation(); },
          unit: function(ip) { return 'Wh/m\u00B2'; },
          steps: 7,  colors: ['#000033','#003399','#0066ff','#66ccff','#ffff00','#ff9900','#ff3300','#ffffff'],
          autoMinMax: false, fixMin: 0, fixMax: null },
        { id: 'diffuseHorizontalRadiation',
          label: 'Diffuse Horiz. Radiation',   defaultChecked: false,
          getVal: function(e, ip) { return e.diffuseHorizontalRadiation(); },
          unit: function(ip) { return 'Wh/m\u00B2'; },
          steps: 7,  colors: ['#000033','#003399','#0066ff','#66ccff','#ffff00','#ff9900','#ff3300','#ffffff'],
          autoMinMax: false, fixMin: 0, fixMax: null },
        { id: 'windSpeed',
          label: 'Wind Speed',                 defaultChecked: false,
          getVal: function(e, ip) { return ip ? convertKnots(e.windSpeed()) : e.windSpeed(); },
          unit: function(ip) { return ip ? 'knots' : 'm/s'; },
          steps: 7,  colors: ['#f7fbff','#deebf7','#c6dbef','#9ecae1','#6baed6','#2171b5','#08306b','#041028'],
          autoMinMax: true },
        { id: 'windDirection',
          label: 'Wind Direction',             defaultChecked: false,
          getVal: function(e, ip) {
              return e.windDirection().map(function(x) { return (+x >= 999) ? null : +x; });
          },
          unit: function(ip) { return '°'; },
          steps: 8,  colors: ['#5e4fa2','#3288bd','#66c2a5','#abdda4','#e6f598','#fee08b','#fdae61','#f46d43','#d53e4f'],
          autoMinMax: false, fixMin: 0, fixMax: 360 },
        { id: 'totalSkyCover',
          label: 'Total Sky Cover',            defaultChecked: false,
          getVal: function(e, ip) { return e.totalSkyCover(); },
          unit: function(ip) { return 'tenths'; },
          steps: 10, colors: ['#6fdcfb','#6bcde9','#68bfd8','#65b1c7','#62a3b6','#5f95a5','#5b8793','#587982','#556b71','#525d60','#4f4f4f'],
          autoMinMax: false, fixMin: 0, fixMax: 10 },
        { id: 'opaqueSkyCover',
          label: 'Opaque Sky Cover',           defaultChecked: false,
          getVal: function(e, ip) { return e.opaqueSkyCover(); },
          unit: function(ip) { return 'tenths'; },
          steps: 10, colors: ['#6fdcfb','#6bcde9','#68bfd8','#65b1c7','#62a3b6','#5f95a5','#5b8793','#587982','#556b71','#525d60','#4f4f4f'],
          autoMinMax: false, fixMin: 0, fixMax: 10 },
        { id: 'visibility',
          label: 'Visibility',                 defaultChecked: false,
          getVal: function(e, ip) { return e.visibility().map(function(x) { return (+x >= 9999) ? null : +x; }); },
          unit: function(ip) { return 'km'; },
          steps: 7,  colors: ['darkblue','blue','cyan','greenyellow','yellow','orange','red','darkred'],
          autoMinMax: true },
        { id: 'ceilingHeight',
          label: 'Ceiling Height',             defaultChecked: false,
          getVal: function(e, ip) { return e.ceilingHeight().map(function(x) { return (+x >= 99999) ? null : (ip ? +x * 3.28084 : +x); }); },
          unit: function(ip) { return ip ? 'ft' : 'm'; },
          steps: 7,  colors: ['darkblue','blue','cyan','greenyellow','yellow','orange','red','darkred'],
          autoMinMax: true },
        { id: 'precipitableWater',
          label: 'Precipitable Water',         defaultChecked: false,
          getVal: function(e, ip) { return e.precipitableWater().map(function(x) { return (+x >= 999) ? null : +x; }); },
          unit: function(ip) { return 'mm'; },
          steps: 7,  colors: ['#ffffcc','#d9f0a3','#addd8e','#78c679','#41ab5d','#238443','#006837','#004529'],
          autoMinMax: true },
        { id: 'aerosolOpticalDepth',
          label: 'Aerosol Optical Depth',      defaultChecked: false,
          getVal: function(e, ip) { return e.aerosolOpticalDepth().map(function(x) { return (+x >= 0.999) ? null : +x; }); },
          unit: function(ip) { return ''; },
          steps: 7,  colors: ['#ffffcc','#d9f0a3','#addd8e','#78c679','#41ab5d','#238443','#006837','#004529'],
          autoMinMax: true },
        { id: 'snowDepth',
          label: 'Snow Depth',                 defaultChecked: false,
          getVal: function(e, ip) { return e.snowDepth().map(function(x) { return (+x >= 999) ? null : (ip ? +x * 0.393701 : +x); }); },
          unit: function(ip) { return ip ? 'in' : 'cm'; },
          steps: 7,  colors: ['#f7fbff','#deebf7','#c6dbef','#9ecae1','#6baed6','#4292c6','#2171b5','#08519c'],
          autoMinMax: true },
        { id: 'daysSinceLastSnowfall',
          label: 'Days Since Last Snowfall',   defaultChecked: false,
          getVal: function(e, ip) { return e.daysSinceLastSnowfall().map(function(x) { return (+x >= 99) ? null : +x; }); },
          unit: function(ip) { return 'days'; },
          steps: 7,  colors: ['#ffffcc','#d9f0a3','#addd8e','#78c679','#41ab5d','#238443','#006837','#004529'],
          autoMinMax: true },
        { id: 'albedo',
          label: 'Albedo',                     defaultChecked: false,
          getVal: function(e, ip) { return e.albedo().map(function(x) { return (+x >= 2) ? null : +x; }); },
          unit: function(ip) { return ''; },
          steps: 7,  colors: ['#ffffd9','#edf8b1','#c7e9b4','#7fcdbb','#41b6c4','#1d91c0','#225ea8','#0c2c84'],
          autoMinMax: true },
        { id: 'liquidPrecipitationDepth',
          label: 'Liquid Precip. Depth',       defaultChecked: false,
          getVal: function(e, ip) { return e.liquidPrecipitationDepth().map(function(x) { return (+x >= 999) ? null : +x; }); },
          unit: function(ip) { return 'mm'; },
          steps: 7,  colors: ['#ffffcc','#d9f0a3','#addd8e','#78c679','#41ab5d','#238443','#006837','#004529'],
          autoMinMax: true },
        { id: 'liquidPrecipitationQuantity',
          label: 'Liquid Precip. Quantity',    defaultChecked: false,
          getVal: function(e, ip) { return e.liquidPrecipitationQuantity().map(function(x) { return (+x >= 99) ? null : +x; }); },
          unit: function(ip) { return 'hr'; },
          steps: 7,  colors: ['#ffffcc','#d9f0a3','#addd8e','#78c679','#41ab5d','#238443','#006837','#004529'],
          autoMinMax: true },
        { id: 'pressure',
          label: 'Station Pressure',           defaultChecked: false,
          getVal: function(e, ip) {
              var v = e.atmosphericStationPressure();
              return v.map(function(x) { return (!x || !isFinite(x)) ? null : (ip ? x * 2.95299830714e-4 : x); });
          },
          unit: function(ip) { return ip ? 'inHg' : 'Pa'; },
          steps: 7,  colors: ['darkblue','blue','cyan','greenyellow','yellow','orange','red','darkred'],
          autoMinMax: true },
        { id: 'pvACOutput',
          label: 'PV AC Output',               defaultChecked: false,
          getVal: function(e, ip) {
              var n = e.dryBulbTemperature().length;
              var h = window._pvHourlyAC;
              var arr = new Array(n);
              for (var i = 0; i < n; i++) { arr[i] = h ? ((h[i] || 0) / 1000) : 0; }
              return arr;
          },
          unit: function(ip) { return 'kWh'; },
          fmt: function(v) { return Number(v).toPrecision(2); },
          steps: 7,  colors: ['#ffffd9','#edf8b1','#c7e9b4','#7fcdbb','#41b6c4','#1d91c0','#225ea8','#0c2c84'],
          autoMinMax: false, fixMin: 0, fixMax: null }
    ];

    // ---- Initialise or restore checkbox selections ----
    if (!window._hmSelectedVars) {
        window._hmSelectedVars = {};
        hmVarDefs.forEach(function(v) { window._hmSelectedVars[v.id] = v.defaultChecked; });
    }

    // ---- Populate checkbox dropdown (clone to strip stale listeners) ----
    var menuEl = document.getElementById('hmDropdownMenu');
    if (!menuEl) return;
    var menuClone = menuEl.cloneNode(false);
    menuEl.parentNode.replaceChild(menuClone, menuEl);
    menuEl = menuClone;
    menuEl.innerHTML = '';

    hmVarDefs.forEach(function(vd) {
        var lbl = document.createElement('label');
        lbl.className = 'hm-checkbox-item';
        var cb = document.createElement('input');
        cb.type    = 'checkbox';
        cb.value   = vd.id;
        cb.checked = !!window._hmSelectedVars[vd.id];
        lbl.appendChild(cb);
        lbl.appendChild(document.createTextNode('\u00A0' + vd.label));
        menuEl.appendChild(lbl);
        cb.addEventListener('change', function() {
            window._hmSelectedVars[vd.id] = this.checked;
            renderHeatmaps();
        });
    });

    // ---- Toggle button (use .onclick to avoid accumulating listeners) ----
    var toggleBtn = document.getElementById('hmDropdownToggle');
    if (toggleBtn) {
        toggleBtn.onclick = function(e) {
            e.stopPropagation();
            menuEl.classList.toggle('open');
        };
    }

    // Close on outside click (store handler so clearEPWCharts can remove it)
    if (window._hmDocClickHandler) {
        document.removeEventListener('click', window._hmDocClickHandler);
    }
    window._hmDocClickHandler = function(e) {
        var dd = document.getElementById('hmDropdown');
        if (dd && !dd.contains(e.target)) {
            menuEl.classList.remove('open');
        }
    };
    document.addEventListener('click', window._hmDocClickHandler);

    // ---- Render selected heatmaps ----
    function renderHeatmaps() {
        var container = document.getElementById('heatmapChartsContainer');
        if (!container) return;
        d3.select('#heatmapChartsContainer').selectAll('svg').remove();
        container.innerHTML = '';
        window._hmColorScales = {};

        hmVarDefs.forEach(function(vd) {
            if (!window._hmSelectedVars[vd.id]) return;

            var rawVals  = vd.getVal(epw, isIP);
            var validArr = rawVals.filter(function(v) { return v !== null && isFinite(v); });
            if (validArr.length === 0) return;

            var minVal = vd.autoMinMax
                ? Math.min.apply(Math, validArr)
                : (vd.fixMin !== undefined && vd.fixMin !== null ? vd.fixMin : Math.min.apply(Math, validArr));
            var maxVal = vd.autoMinMax
                ? Math.max.apply(Math, validArr)
                : (vd.fixMax !== undefined && vd.fixMax !== null ? vd.fixMax : Math.max.apply(Math, validArr));

            // Replace nulls with minVal so every cell gets a colour
            var cleanVals = rawVals.map(function(v) { return (v === null || !isFinite(v)) ? minVal : v; });

            var divId   = 'epwHeatmap-' + vd.id;
            var unitLbl = vd.unit(isIP);

            var wrapDiv  = document.createElement('div');
            wrapDiv.id   = 'wrap-' + divId;

            var h3 = document.createElement('h3');
            h3.innerHTML = vd.label + (unitLbl ? ' (' + unitLbl + ')' : '')
                + ' <button class="btn btn-default btn-xs" style="vertical-align:middle; margin-left:8px;"'
                + ' onclick="downloadSVG(\'' + divId + '\',\'' + vd.id + '-heatmap\')">&#8659; SVG</button>'
                + ' <button class="btn btn-default btn-xs" style="vertical-align:middle;"'
                + ' onclick="downloadPNG(\'' + divId + '\',\'' + vd.id + '-heatmap\')">&#8659; PNG</button>';

            var chartDiv  = document.createElement('div');
            chartDiv.id   = divId;

            wrapDiv.appendChild(h3);
            wrapDiv.appendChild(chartDiv);
            container.appendChild(wrapDiv);

            var hmColors = getHmColors();
            epwHeatmap(epwData(epw, cleanVals), {
                id:          '#' + divId,
                min_value:   minVal,
                max_value:   maxVal,
                unit:        unitLbl,
                steps:       hmColors.length - 1,
                step_colors: hmColors,
                fmtVal:      vd.fmt || null
            });

            // Build and cache the same color scale so the update hook can recolour cells
            var nHmC = hmColors.length;
            var colorVals = d3.range(nHmC).map(function(ci) { return minVal + (maxVal - minVal) * ci / (nHmC - 1); });
            window._hmColorScales[divId] = d3.scale.linear().domain(colorVals).range(hmColors);
        });

        // Re-apply any existing filter state so newly rendered charts are consistent
        if (window._hmLastActiveSet) {
            applyHeatmapFilter(window._hmLastActiveSet, window._hmLastActiveSet.size);
        }
    }

    // ---- Apply a crossfilter selection to all rendered heatmaps ----
    function applyHeatmapFilter(activeSet, n) {
        var pct = totalHours > 0 ? (n / totalHours * 100).toFixed(1) : '0.0';
        var infoEl = document.getElementById('hmFilterInfo');
        if (infoEl) {
            infoEl.textContent = n + ' of ' + totalHours + ' hours (' + pct + '%) selected';
        }
        if (!window._hmColorScales) return;
        Object.keys(window._hmColorScales).forEach(function(divId) {
            var cs = window._hmColorScales[divId];
            if (!cs) return;
            d3.select('#' + divId).selectAll('.heatmap rect')
                .style('fill', function(d) {
                    return activeSet.has(d.index) ? cs(d.value) : '#d0d0d0';
                });
        });
    }

    // ---- Crossfilter update hook (called from renderAll in epwValueFilter) ----
    window.updateHeatmaps = function(filteredData) {
        var activeSet = new Set();
        filteredData.forEach(function(d) { activeSet.add(d.index); });
        window._hmLastActiveSet = activeSet;
        applyHeatmapFilter(activeSet, filteredData.length);
    };

    // ---- PV Recalculate hook: full re-render + reapply filter state ----
    window._hmRerenderAll = function() {
        renderHeatmaps();
        if (window._hmLastActiveSet) applyHeatmapFilter(window._hmLastActiveSet, window._hmLastActiveSet.size);
    };

    renderHeatmaps();
}

//initialization code for the windrose
function epwWindRose(epw) {
    params = {};
    var value = [];
    if (unitSystem == "IP") {
        value = convertKnots(epw.windSpeed());
        params.unit = "knots";
        params.scale_steps = [3.5,6.5,10.5,16.5,21.5,27]; //Beaufort scale in knots
        params.steps = 6;
    } else {
        value = epw.windSpeed();
        params.unit = "m/s";
        params.scale_steps = [1.8,3.3,5.4,8.5,11.1,13.9]; //Beaufort scale in m/s
        params.steps = 6;
    };

    var data = epwData(epw,value); //encoding most of the object construction here
    var direction = epw.windDirection();

    for (var i=0; i < value.length; i++){
        data[i].direction = direction[i];
        data[i].directionGroup = Math.round(direction[i] / 22.5);
        if (data[i].directionGroup == 0) { //0 and 360 are the same
            data[i].directionGroup = 16;
        };
        if (data[i].value == 0) { //0 wind speed is 0 group
            data[i].directionGroup = 0;
        };
    };

    params.id = "#epwWindRose";
    params.min_value = 0;
    params.max_value = Math.max.apply(Math,value);
    params.length = value.length;
    params.directions = 16;
    params.labels = ['NNE','NE','ENE','E','ESE','SE','SSE','S','SSW','SW','WSW','W','WNW','NW','NNW','N'];
    params.step_colors = ['#d73027','#fc8d59','#fee090','#e0f3f8','#91bfdb','#4575b4'];
    params.legend_text = ['Light Air','Light Breeze','Gentle Breeze','Moderate Breeze','Fresh Breeze','Strong Breeze'];

    epwRadialChart(data,params)
};

//general code for making a radial chart
function epwRadialChart(data,params) {
    //references
    //http://sustainabilityworkshop.autodesk.com/buildings/wind-rose-diagrams
    //http://bl.ocks.org/nbremer/6506614
    //http://bl.ocks.org/chrisrzhou/2421ac6541b68c1680f8
    //add Beaufort scale

    var min_value = params.min_value,
    max_value = params.max_value,
    steps = params.steps,   //number of steps in color scale
    scale_steps = params.scale_steps,
    legend_scale = [],
    color_values = [];

    var colorScale = d3.scale.ordinal()
        .domain(scale_steps)
        .range(params.step_colors)

    //make a new data group based on the scale_steps
    for (var i=0; i < params.length; i++){
        for (var j=0; j < steps; j++) {
            data[i].scaleStep = j;
            if (data[i].value < scale_steps[j]) { break; }
        };
    };

    //bin the data by direction and scale_step
    var mapped_data = d3.nest()
      .key(function(d) { return d.directionGroup; })
      .key(function(d) { return d.scaleStep; })
      .rollup(function(v) { return v.length; })
      .map(data);
    //console.log(JSON.stringify(mapped_data));
    //console.log(mapped_data[1])

    // Count calm hours directly (directionGroup === 0 means windSpeed === 0)
    var zero_num = 0;
    for (var i = 0; i < data.length; i++) {
        if (data[i].directionGroup === 0) { zero_num++; }
    }
    var zero_frac = params.length > 0 ? zero_num / params.length : 0;

    //take nested data and transform into arc_data, and get max radius for scaling
    var arc_data = [],
        max_radius = 0;
    for (var i=1; i <= params.directions; i++) {
        var c = mapped_data[i];
        if (!c) { continue; } // no records in this direction
        var prior_radius = 0;
        for (var j=0; j < steps; j++) {
            //skip making arc if no data in that step
            if (c[j] == null) { continue; }
            //do something here to build the arc data object
            var arc = [];
            arc.directionGroup = i;
            arc.scaleStep = j;
            arc.innerRadius = prior_radius;
            arc.outerRadius = prior_radius + c[j];
            if (arc.outerRadius > max_radius) { max_radius = arc.outerRadius};
            arc_data.push(arc);
            prior_radius = arc.outerRadius;
        };
    };
   //console.log(arc_data);

    // define svg size
    var margin = {top: 20, right: 220, bottom: 20, left: 20},
    width = 700 - margin.left - margin.right,
    height = 540 - margin.top - margin.bottom,
    cx = width/2,
    cy = height/2,
    radius = Math.min(cx,cy);

    var svg = d3.select(params.id).append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
        .append("g")
        .attr("transform", "translate(" + (cx + margin.left) + "," + (cy + margin.top) +")");

    //still need to add frequency axis, lines for now
    var lines = svg.append("g").selectAll("line")
        .data(params.labels)
      .enter().append("line")
        .attr("class","label-line")
        .attr("y2", -0.85*radius)
        .style("stroke", "black") //include in css file
        .style("stroke-width",".5px") //include in css file
        .attr("transform", function(d, i) { return "rotate(" + (i * 360 / params.directions) + ")"; });

    var labels = svg.append("g").selectAll("text")
        .data(params.labels)
      .enter().append("text")
        .attr("class","label")
        .attr("text-anchor", "middle")
        .attr("x", function(d, i) { return 0.9*radius * Math.sin((i+1)*2*Math.PI/params.directions); })
        .attr("y", function(d, i) { return 0.9*radius * -Math.cos((i+1)*2*Math.PI/params.directions); })
        .text( function(d, i) { return d; });

    var scaling_factor = max_radius > 0 ? (0.8*radius)/max_radius : 0; //set max_radius to 1
    var arc = d3.svg.arc()
        .outerRadius(function(d) { return (d.outerRadius*scaling_factor);})
        .innerRadius(function(d) { return (d.innerRadius*scaling_factor);})
        .startAngle(function(d) { return (d.directionGroup * (2*Math.PI/params.directions)) - (Math.PI/params.directions);})
        .endAngle(function(d) { return (d.directionGroup * (2*Math.PI/params.directions)) + (Math.PI/params.directions);});

    var arcs = svg.selectAll('path')
        .data(arc_data)
      .enter().append("path")
        .attr("d", arc)
        .style("fill", function(d) { return colorScale(d.scaleStep); })
        .style("stroke","white")
        //.on('mouseover', function(d) {return console.log("directionGroup:" + d.directionGroup + ",scaleStep:" + d.scaleStep);}); //tooltip here

        //legend element
    var legend = svg.append("g")
        .attr("class","legend")
        .attr("transform", "translate(" + cx + "," + (-cy) + ")")

    //color legend for color scale
    legend.selectAll("rect")
        .data(params.step_colors)
      .enter().append("rect")
        .attr("x", 0)
        .attr("y", function(d,i) { return cy - 40 - i*20; })
        .attr("width", 15)
        .attr("height", 15)
        .style("fill", function(d) { return d; })

    //text label for the color scale
    legend.selectAll("text")
        .data(params.scale_steps)
     .enter().append("text")
        .style("text-anchor", "left")
        .attr("x", 20)
        .attr("y", function(d,i) {return cy - 40 - i*20; } )
        .attr("dy", "1em")
        .text(function(d,i) {
            var label = "";
            if (i == 0) { label = "0 - " + d + " " + params.unit + ", " + params.legend_text[i]; }
            else { label = params.scale_steps[i-1] + " - " + d + " " + params.unit + ", " + params.legend_text[i]; }
            return label;
        });

    //Beaufort Scale
    svg.append("g")
      .attr("class","legend-header")
      .attr("transform", "translate(" + (cx) + "," + 0 + ")")
      .append("text")
      .style("text-anchor", "left")
      .text("Beaufort Scale")

    //number of selected and calm hours
    var formatPct = d3.format(".1%");
    var totalLength = params.totalLength || params.length;
    var selected_frac = totalLength > 0 ? params.length / totalLength : 0;

    var infoGroup = svg.append("g")
      .attr("class","legend-header")
      .attr("transform", "translate(" + (-cx) + "," + (-(cy - 15)) + ")");

    infoGroup.append("text")
      .attr("y", 0)
      .style("text-anchor", "start")
      .text(params.length + " of " + totalLength + " hours (" + formatPct(selected_frac) + ") selected");

    infoGroup.append("text")
      .attr("y", 16)
      .style("text-anchor", "start")
      .text(zero_num + " of " + params.length + " selected hours (" + formatPct(zero_frac) + ") calm");
};

//general code for making a heatmap
function epwHeatmap(data,params) {
    var min_value = params.min_value,
        max_value = params.max_value,
        steps = params.steps,   //number of steps in color scale
        scale_step = (max_value - min_value)/steps,
        legend_step = (max_value - min_value)/(steps+1),
        legend_scale = [],
        color_values = [];

    for (var i=0; i < steps + 2; i++) {
        var step = min_value + i*legend_step;
        legend_scale[i] = (params.fmtVal ? params.fmtVal(step) : step.toFixed(1)) + params.unit;
    };

    //define color map
    for (var i = 0; i < steps + 1; i++) {
        color_values.push(min_value + scale_step*i);
    }
    var colorScale = d3.scale.linear()
        .domain(color_values)
        .range(params.step_colors);

    //define grid and svg
    var gridSize = 30,
        h = gridSize/2,     //height of each row in the heatmap
        w = gridSize/15,    //width of each column in the heatmap
        rectPadding = 0;

    var margin = {top: 10, right: 120, bottom: 40, left: 40},
        width = w*366, //extra day to account for leap years
        height = h*24;

    var svg = d3.select(params.id).append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom);

    //heatmap
    svg.append("g")
        .attr("class", "heatmap")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")")
        .selectAll("rect")
        .data(data, function(d) { return d.dayOfYear + ':' + d.hour; })
      .enter().append("rect")
        .attr("x", function(d) { return d.dayOfYear * w; })
        .attr("y", function(d,i) { return (d.hour-1) * h; })
        .attr("width", function(d) { return w; })
        .attr("height", function(d) { return h; })
        .style("fill", function(d) { return colorScale(d.value); })
        .on('mouseover', function(d) {
            var tt = document.getElementById('epw-heatmap-tooltip');
            if (!tt) return;
            var mNames = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
            var tLabels = ["12am","1am","2am","3am","4am","5am","6am","7am","8am","9am","10am","11am",
                           "12pm","1pm","2pm","3pm","4pm","5pm","6pm","7pm","8pm","9pm","10pm","11pm"];
            var mName  = mNames[d.month - 1]  || ('M' + d.month);
            var hLabel = tLabels[d.hour - 1]  || (d.hour + ':00');
            tt.innerHTML = '<strong>' + mName + ' ' + d.day + '</strong>&nbsp;&nbsp;' + hLabel
                + '<br>' + (typeof d.value === 'number' ? (params.fmtVal ? params.fmtVal(d.value) : d.value.toFixed(1)) : d.value) + '\u00A0' + params.unit;
            tt.style.display = 'block';
            tt.style.left = (d3.event.clientX + 12) + 'px';
            tt.style.top  = (d3.event.clientY + 14) + 'px';
        })
        .on('mousemove', function() {
            var tt = document.getElementById('epw-heatmap-tooltip');
            if (!tt) return;
            tt.style.left = (d3.event.clientX + 12) + 'px';
            tt.style.top  = (d3.event.clientY + 14) + 'px';
        })
        .on('mouseout', function() {
            var tt = document.getElementById('epw-heatmap-tooltip');
            if (tt) tt.style.display = 'none';
        });

    //legend element
    var legend = svg.append("g")
        .attr("class","legend")
        .attr("transform", "translate(" + (width + margin.left) + "," + margin.top + ")")

    // color legend for color scale — anchored to y-axis tick extent [0, height]
    legend.selectAll("rect")
        .data(colorScale.domain())
      .enter().append("rect")
        .attr("x", 5)
        .attr("y", function(d,i) {return height - (h*24/(steps+1))*(i+1); } )
        .attr("width", 15)
        .attr("height", h*24/(steps+1))
        .style("fill", function(d) {return colorScale(d); })

    // text label for the color scale
    legend.selectAll("text")
        .data(legend_scale)
     .enter().append("text")
        .style("text-anchor", "left")
        .attr("x", 20)
        .attr("y", function(d,i) {return height - (h*24/(steps+1))*i; } )
        .attr("dy", "0.35em")
        .text(function(d, i) { return legend_scale[i]; });

    // add times scale to the figure
    var times = ["12am","1am","2am","3am","4am","5am","6am","7am","8am","9am","10am","11am","12pm","1pm","2pm","3pm","4pm","5pm","6pm","7pm","8pm","9pm","10pm","11pm","12am"];
    var y = d3.scale.linear()
            .range([0, height])
            .domain([0, 24]),
        yAxis = d3.svg.axis()
            .orient("left")
            .scale(y)
            .ticks(25)
            .tickFormat( function(d,i) { return times[i]; });
    svg.append("g")
        .attr("class", "axis")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")")
        .call(yAxis);

    var data_month = [{"label":"Jan"},{"label":"Feb"},{"label":"Mar"},{"label":"April"},{"label":"May"},{"label":"June"},{"label":"July"},{"label":"Aug"},{"label":"Sept"},{"label":"Oct"},{"label":"Nov"},{"label":"Dec"}];

    // text label for month
    svg.append("g")
        .attr("class", "axis")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")")
        .selectAll("text")
        .data(data_month)
      .enter().append("text")
        .style("text-anchor", "middle")
        .attr("x", function(d,i) {return width*i/12 + w*15; } )
        .attr("y", height + 20)
        .text(function(d, i) { return data_month[i].label; });
};

//initialization code for the drybulb temperature crossfilter
function epwValueFilter(epw) {
    // Clear any previously-rendered SVGs so chart skeletons (including reset links) are
    // fully rebuilt on every call (e.g. after Recalculate).
    d3.selectAll(".xfilterChart svg").remove();

    var data = epwDataDefault(epw,unitSystem); //encoding most of the object construction here
    //console.log(data);

    var formatNumber = d3.format(",d"),
        formatPercent = d3.format(".2%d");

    var xfilter = crossfilter(data),
        all = xfilter.groupAll(),
        month = xfilter.dimension(function(d) { return d.month; }),
        months = month.group(Math.floor);
        hour = xfilter.dimension(function(d) { return d.hour; }),
        hours = hour.group(Math.floor),
        dryBulbTemperature = xfilter.dimension(function(d) { return d.dryBulbTemperature; }),
        tempArray = epw.dryBulbTemperature();
        if (unitSystem == "IP") { tempArray = convertCtoF(tempArray) };
    var dryBulbTemperatureMin = Math.min.apply(Math,tempArray),
        dryBulbTemperatureMax = Math.max.apply(Math,tempArray),
        dryBulbTemperatureLowX = Math.floor(dryBulbTemperatureMin / 5)*5,
        dryBulbTemperatureHighX = Math.floor((dryBulbTemperatureMax + 5) / 5)*5,
        // Dewpoint is derived from vapor pressure (consistent with humidity ratio on
        // the psychrometric chart) rather than EPW field 7, so the filter and chart agree.
        dewPointTemperature = xfilter.dimension(function(d) {
            if (d.vaporPressure === null || !isFinite(d.vaporPressure) || d.vaporPressure <= 0) return -999;
            var lnPv = Math.log(d.vaporPressure / 611.657);
            var tdC = 243.04 * lnPv / (17.625 - lnPv);
            return unitSystem == "IP" ? 32 + tdC * 1.8 : tdC;
        }),
        tempArray = data
            .filter(function(d) { return d.vaporPressure !== null && isFinite(d.vaporPressure) && d.vaporPressure > 0; })
            .map(function(d) {
                var lnPv = Math.log(d.vaporPressure / 611.657);
                var tdC = 243.04 * lnPv / (17.625 - lnPv);
                return unitSystem == "IP" ? 32 + tdC * 1.8 : tdC;
            });
    var dewPointTemperatureMin = Math.min.apply(Math,tempArray),
        dewPointTemperatureMax = Math.max.apply(Math,tempArray),
        dewPointTemperatureLowX = Math.floor(dewPointTemperatureMin / 5)*5,
        dewPointTemperatureHighX = Math.floor((dewPointTemperatureMax + 5) / 5)*5,
        relativeHumidity = xfilter.dimension(function(d) { return d.relativeHumidity; }),
        tempArray = epw.relativeHumidity(),
        relativeHumidityMin = Math.min.apply(Math,tempArray),
        relativeHumidityMax = Math.max.apply(Math,tempArray),
        relativeHumidityLowX = Math.floor(relativeHumidityMin / 5)*5,
        relativeHumidityHighX = Math.floor((relativeHumidityMax + 5) / 5)*5,
        relativeHumiditys = relativeHumidity.group(Math.floor),
        outputDim = xfilter.dimension(function(d) { return d.index; });

    // Precompute wind rose direction groups on the crossfilter data
    for (var i = 0; i < data.length; i++) {
        data[i].directionGroup = Math.round(data[i].windDirection / 22.5);
        if (data[i].directionGroup === 0) { data[i].directionGroup = 16; }
        if (data[i].windSpeed === 0) { data[i].directionGroup = 0; }
    }
    var wrScaleSteps = unitSystem == "IP" ? [3.5,6.5,10.5,16.5,21.5,27] : [1.8,3.3,5.4,8.5,11.1,13.9];
    var wrUnit = unitSystem == "IP" ? "knots" : "m/s";
    var wrMaxValue = Math.max.apply(Math, data.map(function(d) { return d.windSpeed; }));

    window.updateWindRose = function(filteredData) {
        d3.select("#epwWindRose").selectAll("svg").remove();
        if (!filteredData) { filteredData = []; }
        var wrData = [];
        for (var j = 0; j < filteredData.length; j++) {
            wrData.push({
                index: filteredData[j].index,
                directionGroup: filteredData[j].directionGroup,
                value: filteredData[j].windSpeed
            });
        }
        var wrParams = {
            id: "#epwWindRose",
            min_value: 0,
            max_value: wrMaxValue,
            length: wrData.length,
            totalLength: data.length,
            directions: 16,
            steps: 6,
            scale_steps: wrScaleSteps,
            unit: wrUnit,
            labels: ['NNE','NE','ENE','E','ESE','SE','SSE','S','SSW','SW','WSW','W','WNW','NW','NNW','N'],
            step_colors: ['#d73027','#fc8d59','#fee090','#e0f3f8','#91bfdb','#4575b4'],
            legend_text: ['Light Air','Light Breeze','Gentle Breeze','Moderate Breeze','Fresh Breeze','Strong Breeze']
        };
        epwRadialChart(wrData, wrParams);
    };

    if (unitSystem == "IP") {
        dryBulbTemperatures = dryBulbTemperature.group(function(d) { return Math.floor(d / 2) * 2; }); // (d / 2) * 2
        dewPointTemperatures = dewPointTemperature.group(function(d) { return Math.floor(d / 2) * 2; }); // (d / 2) * 2
    } else {
        dryBulbTemperatures = dryBulbTemperature.group(function(d) { return Math.floor(d); });
        dewPointTemperatures = dewPointTemperature.group(function(d) { return Math.floor(d); });
    };

    // Wet bulb temperature dimension
    var wetBulbTemperature = xfilter.dimension(function(d) {
        if (d.wetBulbTemperature === null || !isFinite(d.wetBulbTemperature)) return -999;
        return unitSystem == "IP" ? 32 + d.wetBulbTemperature * 1.8 : d.wetBulbTemperature;
    });
    var wbArray = epw.wetBulbTemperature().filter(function(v) { return v !== null && isFinite(v); });
    if (unitSystem == "IP") { wbArray = convertCtoF(wbArray); }
    var wetBulbTemperatureLowX = Math.floor(Math.min.apply(Math, wbArray) / 5)*5,
        wetBulbTemperatureHighX = Math.floor((Math.max.apply(Math, wbArray) + 5) / 5)*5;
    var wetBulbTemperatures = unitSystem == "IP"
        ? wetBulbTemperature.group(function(d) { return Math.floor(d / 2) * 2; })
        : wetBulbTemperature.group(function(d) { return Math.floor(d); });

    // Enthalpy dimension (kJ/kg SI, BTU/lb IP)
    var enthalpyDim = xfilter.dimension(function(d) {
        if (d.moistAirEnthalpy === null || !isFinite(d.moistAirEnthalpy)) return -999;
        return d.moistAirEnthalpy * (unitSystem == "IP" ? 0.000429923 : 0.001);
    });
    var enthArray = epw.moistAirEnthalpy()
        .filter(function(v) { return v !== null && isFinite(v); })
        .map(function(v) { return v * (unitSystem == "IP" ? 0.000429923 : 0.001); });
    var enthalpyLowX = Math.floor(Math.min.apply(Math, enthArray) / 5)*5,
        enthalpyHighX = Math.floor((Math.max.apply(Math, enthArray) + 5) / 5)*5;
    var enthalpyStep = unitSystem == "IP" ? 1 : 5;
    var enthalpys = enthalpyDim.group(function(d) { return Math.floor(d / enthalpyStep) * enthalpyStep; });

    // Humidity ratio dimension
    var hrDim = xfilter.dimension(function(d) {
        if (d.humidityRatio === null || !isFinite(d.humidityRatio)) return -999;
        return d.humidityRatio * (unitSystem == "IP" ? 7000 : 1000);
    });
    var hrArr = data.filter(function(d) { return d.humidityRatio !== null && isFinite(d.humidityRatio); })
        .map(function(d) { return d.humidityRatio * (unitSystem == "IP" ? 7000 : 1000); });
    var hrMax = hrArr.length ? Math.ceil(Math.max.apply(Math, hrArr)) : (unitSystem == "IP" ? 200 : 30);
    var hrStep = unitSystem == "IP" ? 2 : 0.5;
    var hRatios = hrDim.group(function(d) { return d < 0 ? 0 : Math.floor(d / hrStep) * hrStep; });

    // Radiation dimensions (Wh/m², no IP conversion)
    var ghiDim = xfilter.dimension(function(d) {
        return (d.globalHorizontalRadiation === null || !isFinite(d.globalHorizontalRadiation)) ? -999 : d.globalHorizontalRadiation;
    });
    var ghiMax = Math.ceil(Math.max.apply(Math, data.map(function(d) { return d.globalHorizontalRadiation || 0; })) / 25) * 25 || 1000;
    var ghis = ghiDim.group(function(d) { return d < 0 ? 0 : Math.floor(d / 25) * 25; });

    var dniDim = xfilter.dimension(function(d) {
        return (d.directNormalRadiation === null || !isFinite(d.directNormalRadiation)) ? -999 : d.directNormalRadiation;
    });
    var dniMax = Math.ceil(Math.max.apply(Math, data.map(function(d) { return d.directNormalRadiation || 0; })) / 25) * 25 || 1000;
    var dnis = dniDim.group(function(d) { return d < 0 ? 0 : Math.floor(d / 25) * 25; });

    var dhiDim = xfilter.dimension(function(d) {
        return (d.diffuseHorizontalRadiation === null || !isFinite(d.diffuseHorizontalRadiation)) ? -999 : d.diffuseHorizontalRadiation;
    });
    var dhiMax = Math.ceil(Math.max.apply(Math, data.map(function(d) { return d.diffuseHorizontalRadiation || 0; })) / 25) * 25 || 600;
    var dhis = dhiDim.group(function(d) { return d < 0 ? 0 : Math.floor(d / 25) * 25; });

    // Wind speed dimension
    var windSpeedDim = xfilter.dimension(function(d) { return d.windSpeed; });
    var wsMax = Math.ceil(Math.max.apply(Math, data.map(function(d) { return d.windSpeed; })));
    var wsStep = unitSystem == "IP" ? 2 : 1;
    var windSpeeds = windSpeedDim.group(function(d) { return Math.floor(d / wsStep) * wsStep; });

    // Wind direction dimension (1° resolution, 0-359); calms (windSpeed=0) excluded from counts
    var windDirDim = xfilter.dimension(function(d) {
        var dir = +d.windDirection;
        return (!isFinite(dir) || dir > 380) ? 0 : Math.floor(dir);
    });
    // Wrap the group so .all() always returns all 360 bins regardless of EPW resolution
    var windDirGroup = (function() {
        var raw = windDirDim.group(Math.floor)
            .reduce(
                function(p, v) { return p + (v.windSpeed > 0 ? 1 : 0); },
                function(p, v) { return p - (v.windSpeed > 0 ? 1 : 0); },
                function()     { return 0; }
            );
        return {
            all: function() {
                var map = {};
                raw.all().forEach(function(d) { map[d.key] = d.value; });
                var result = [];
                for (var i = 0; i < 360; i++) {
                    result.push({ key: i, value: map[i] || 0 });
                }
                return result;
            }
        };
    }());

    // Sky cover dimension (0-10 tenths)
    var skyDim = xfilter.dimension(function(d) { return d.totalSkyCover; });
    var skys = skyDim.group(Math.floor);

    // Opaque sky cover dimension (0-10 tenths)
    var opaqueSkyCoverDim = xfilter.dimension(function(d) { return d.opaqueSkyCover; });
    var opaqueSkys = opaqueSkyCoverDim.group(Math.floor);

    // Visibility dimension (km)
    var visibilityDim = xfilter.dimension(function(d) { return d.visibility === null ? -999 : d.visibility; });
    var visArr = data.filter(function(d) { return d.visibility !== null && isFinite(d.visibility); }).map(function(d) { return d.visibility; });
    var visMax = visArr.length ? Math.ceil(Math.max.apply(Math, visArr) / 5) * 5 : 50;
    var visGroup = visibilityDim.group(function(d) { return d < 0 ? 0 : Math.floor(d / 2) * 2; });

    // Ceiling height dimension (m SI / ft IP)
    var ceilingConv = unitSystem == 'IP' ? 3.28084 : 1;
    var ceilingDim = xfilter.dimension(function(d) { return d.ceilingHeight === null ? -999 : d.ceilingHeight * ceilingConv; });
    var ceilArr = data.filter(function(d) { return d.ceilingHeight !== null && isFinite(d.ceilingHeight); }).map(function(d) { return d.ceilingHeight * ceilingConv; });
    var ceilMax = ceilArr.length ? Math.ceil(Math.max.apply(Math, ceilArr) / 100) * 100 : (unitSystem == 'IP' ? 25000 : 7500);
    var ceilStep = unitSystem == 'IP' ? 500 : 150;
    var ceilGroup = ceilingDim.group(function(d) { return d < 0 ? 0 : Math.floor(d / ceilStep) * ceilStep; });

    // Precipitable water dimension (mm)
    var precipWaterDim = xfilter.dimension(function(d) { return d.precipitableWater === null ? -999 : d.precipitableWater; });
    var pwArr = data.filter(function(d) { return d.precipitableWater !== null && isFinite(d.precipitableWater); }).map(function(d) { return d.precipitableWater; });
    var pwMax = pwArr.length ? Math.ceil(Math.max.apply(Math, pwArr) / 5) * 5 : 100;
    var pwGroup = precipWaterDim.group(function(d) { return d < 0 ? 0 : Math.floor(d / 2) * 2; });

    // Aerosol optical depth dimension (dimensionless)
    var aodDim = xfilter.dimension(function(d) { return d.aerosolOpticalDepth === null ? -999 : d.aerosolOpticalDepth; });
    var aodArr = data.filter(function(d) { return d.aerosolOpticalDepth !== null && isFinite(d.aerosolOpticalDepth); }).map(function(d) { return d.aerosolOpticalDepth; });
    var aodMax = aodArr.length ? Math.ceil(Math.max.apply(Math, aodArr) * 100) / 100 : 0.5;
    var aodGroup = aodDim.group(function(d) { return d < 0 ? 0 : Math.floor(d * 100) / 100; });

    // Snow depth dimension (cm SI / in IP)
    var snowConv = unitSystem == 'IP' ? 0.393701 : 1;
    var snowDim = xfilter.dimension(function(d) { return d.snowDepth === null ? -999 : d.snowDepth * snowConv; });
    var snowArr = data.filter(function(d) { return d.snowDepth !== null && isFinite(d.snowDepth); }).map(function(d) { return d.snowDepth * snowConv; });
    var snowMax = snowArr.length ? Math.ceil(Math.max.apply(Math, snowArr)) : (unitSystem == 'IP' ? 60 : 150);
    var snowGroup = snowDim.group(function(d) { return d < 0 ? 0 : Math.floor(d); });

    // Days since last snowfall dimension
    var dslsDim = xfilter.dimension(function(d) { return d.daysSinceLastSnowfall === null ? -999 : d.daysSinceLastSnowfall; });
    var dslsArr = data.filter(function(d) { return d.daysSinceLastSnowfall !== null && isFinite(d.daysSinceLastSnowfall); }).map(function(d) { return d.daysSinceLastSnowfall; });
    var dslsMax = dslsArr.length ? Math.ceil(Math.max.apply(Math, dslsArr)) : 88;
    var dslsGroup = dslsDim.group(function(d) { return d < 0 ? 0 : Math.floor(d); });

    // Albedo dimension (dimensionless)
    var albedoDim = xfilter.dimension(function(d) { return d.albedo === null ? -999 : d.albedo; });
    var albArr = data.filter(function(d) { return d.albedo !== null && isFinite(d.albedo); }).map(function(d) { return d.albedo; });
    var albMax = albArr.length ? Math.ceil(Math.max.apply(Math, albArr) * 20) / 20 : 1.0;
    var albGroup = albedoDim.group(function(d) { return d < 0 ? 0 : Math.floor(d * 20) / 20; });

    // Liquid precipitation depth dimension (mm)
    var lpdDim = xfilter.dimension(function(d) { return d.liquidPrecipitationDepth === null ? -999 : d.liquidPrecipitationDepth; });
    var lpdArr = data.filter(function(d) { return d.liquidPrecipitationDepth !== null && isFinite(d.liquidPrecipitationDepth); }).map(function(d) { return d.liquidPrecipitationDepth; });
    var lpdMax = lpdArr.length ? Math.ceil(Math.max.apply(Math, lpdArr)) : 100;
    var lpdGroup = lpdDim.group(function(d) { return d < 0 ? 0 : Math.floor(d); });

    // Liquid precipitation quantity (collection interval, hr)
    var lpqDim = xfilter.dimension(function(d) { return d.liquidPrecipitationQuantity === null ? -999 : d.liquidPrecipitationQuantity; });
    var lpqArr = data.filter(function(d) { return d.liquidPrecipitationQuantity !== null && isFinite(d.liquidPrecipitationQuantity); }).map(function(d) { return d.liquidPrecipitationQuantity; });
    var lpqMax = lpqArr.length ? Math.ceil(Math.max.apply(Math, lpqArr)) : 24;
    var lpqGroup = lpqDim.group(function(d) { return d < 0 ? 0 : Math.floor(d); });

    // Atmospheric pressure dimension
    var presDim = xfilter.dimension(function(d) {
        if (d.atmosphericStationPressure === null || !isFinite(d.atmosphericStationPressure)) return -999;
        return unitSystem == "IP" ? d.atmosphericStationPressure * 2.95299830714e-4 : d.atmosphericStationPressure;
    });
    var presArr = data.filter(function(d) { return d.atmosphericStationPressure !== null && isFinite(d.atmosphericStationPressure); })
        .map(function(d) { return unitSystem == "IP" ? d.atmosphericStationPressure * 2.95299830714e-4 : d.atmosphericStationPressure; });
    var presStep = unitSystem == "IP" ? 0.05 : 50;
    var presLowX  = presArr.length ? Math.floor(Math.min.apply(Math, presArr) / presStep) * presStep : 0;
    var presHighX = presArr.length ? Math.ceil(Math.max.apply(Math, presArr)  / presStep) * presStep : (unitSystem == "IP" ? 31 : 105000);
    var presGroup = presDim.group(function(d) { return d < 0 ? presLowX : Math.floor(d / presStep) * presStep; });

    // PV AC output dimension (kWh, computed by epwSolarPV)
    var pvAcDim = xfilter.dimension(function(d) { return d.pvAC_kWh; });
    var pvAcArr = data.map(function(d) { return d.pvAC_kWh || 0; });
    var pvAcMax = Math.ceil(Math.max.apply(Math, pvAcArr) * 2) / 2 || 5;
    var pvAcStep = 0.05;
    var pvAcGroup = pvAcDim.group(function(d) { return d < 0 ? 0 : Math.floor(d / pvAcStep) * pvAcStep; });

    // Update chart titles with units based on unit system
    var tempUnit = unitSystem == "IP" ? "(\xB0F)" : "(\xB0C)";
    d3.select("#drybulb-temperature-chart .title").text("Drybulb Temperature " + tempUnit);
    d3.select("#dewpoint-temperature-chart .title").text("Dewpoint Temperature " + tempUnit);
    d3.select("#wetbulb-temperature-chart .title").text("Wetbulb Temperature " + tempUnit);
    d3.select("#rh-chart .title").text("Relative Humidity (%)");
    d3.select("#enthalpy-chart .title").text(unitSystem == "IP" ? "Enthalpy (BTU/lb)" : "Enthalpy (kJ/kg)");
    d3.select("#humidity-ratio-chart .title").text(unitSystem == "IP" ? "Humidity Ratio (gr/lb)" : "Humidity Ratio (g/kg)");
    d3.select("#ghi-chart .title").text("Global Horiz. Radiation (Wh/m\u00B2)");
    d3.select("#dni-chart .title").text("Direct Normal Radiation (Wh/m\u00B2)");
    d3.select("#dhi-chart .title").text("Diffuse Horiz. Radiation (Wh/m\u00B2)");
    d3.select("#wind-speed-chart .title").text(unitSystem == "IP" ? "Wind Speed (knots)" : "Wind Speed (m/s)");
    d3.select("#wind-direction-chart .title").text("Wind Direction (°)");
    d3.select("#sky-cover-chart .title").text("Total Sky Cover (tenths)");
    d3.select("#opaque-sky-cover-chart .title").text("Opaque Sky Cover (tenths)");
    d3.select("#visibility-chart .title").text("Visibility (km)");
    d3.select("#ceiling-height-chart .title").text(unitSystem == "IP" ? "Ceiling Height (ft)" : "Ceiling Height (m)");
    d3.select("#precipitable-water-chart .title").text("Precipitable Water (mm)");
    d3.select("#aerosol-optical-depth-chart .title").text("Aerosol Optical Depth");
    d3.select("#snow-depth-chart .title").text(unitSystem == "IP" ? "Snow Depth (in)" : "Snow Depth (cm)");
    d3.select("#days-since-last-snowfall-chart .title").text("Days Since Last Snowfall");
    d3.select("#albedo-chart .title").text("Albedo");
    d3.select("#liquid-precip-depth-chart .title").text("Liquid Precip. Depth (mm)");
    d3.select("#liquid-precip-qty-chart .title").text("Liquid Precip. Quantity (hr)");
    d3.select("#pressure-chart .title").text(unitSystem == "IP" ? "Station Pressure (inHg)" : "Station Pressure (Pa)");
    d3.select("#pv-ac-chart .title").text("PV AC Output (kWh)");

    // Reset shared chart ID counter so reset(i) indices align with charts[] indices
    window._epwChartIdCounter = 0;

    var charts = [

        epwPolarChart()
            .dimension(month)
            .group(months)
            .radius(80)
            .innerRadius(22)
            .showRangeInputs(false)
            .labels(["Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"]),

        epwPolarChart()
            .dimension(hour)
            .group(hours)
            .radius(80)
            .innerRadius(22)
            .showRangeInputs(false)
            .labelOffset(0)
            .labels(["12a","1a","2a","3a","4a","5a","6a","7a","8a","9a","10a","11a","12p","1p","2p","3p","4p","5p","6p","7p","8p","9p","10p","11p"]),

        epwBarChart()
            .dimension(dryBulbTemperature)
            .group(dryBulbTemperatures)
          .x(d3.scale.linear()
            .domain([dryBulbTemperatureLowX,dryBulbTemperatureHighX])
            .rangeRound([0, 10 * 45])), //make sure pixel width plus margins matches epwvis.css (sidebar width)

        epwBarChart()
            .dimension(dewPointTemperature)
            .group(dewPointTemperatures)
          .x(d3.scale.linear()
            .domain([dewPointTemperatureLowX,dewPointTemperatureHighX])
            .rangeRound([0, 10 * 45])), //make sure pixel width plus margins matches epwvis.css (sidebar width)

        epwBarChart()
            .dimension(relativeHumidity)
            .group(relativeHumiditys)
          .x(d3.scale.linear()
            .domain([relativeHumidityLowX,relativeHumidityHighX])
            .rangeRound([0, 10 * 45])), //make sure pixel width plus margins matches epwvis.css (sidebar width)

        epwBarChart()
            .dimension(wetBulbTemperature)
            .group(wetBulbTemperatures)
          .x(d3.scale.linear()
            .domain([wetBulbTemperatureLowX,wetBulbTemperatureHighX])
            .rangeRound([0, 10 * 45])), //make sure pixel width plus margins matches epwvis.css (sidebar width)

        epwBarChart()
            .dimension(enthalpyDim)
            .group(enthalpys)
          .x(d3.scale.linear()
            .domain([enthalpyLowX,enthalpyHighX])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(hrDim)
            .group(hRatios)
          .x(d3.scale.linear()
            .domain([0, hrMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(ghiDim)
            .group(ghis)
          .x(d3.scale.linear()
            .domain([0, ghiMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(dniDim)
            .group(dnis)
          .x(d3.scale.linear()
            .domain([0, dniMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(dhiDim)
            .group(dhis)
          .x(d3.scale.linear()
            .domain([0, dhiMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(windSpeedDim)
            .group(windSpeeds)
          .x(d3.scale.linear()
            .domain([0, wsMax])
            .rangeRound([0, 10 * 45])),

        epwPolarChart()
            .dimension(windDirDim)
            .group(windDirGroup)
            .radius(145)
            .innerRadius(30)
            .labelOffset(0.5)
            .labels((function() {
                var l = [];
                for (var i = 0; i < 360; i++) { l.push(i % 15 === 0 ? String(i) + '\u00b0' : ''); }
                return l;
            }())),

        epwBarChart()
            .dimension(skyDim)
            .group(skys)
          .x(d3.scale.linear()
            .domain([0, 10])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(opaqueSkyCoverDim)
            .group(opaqueSkys)
          .x(d3.scale.linear()
            .domain([0, 10])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(visibilityDim)
            .group(visGroup)
          .x(d3.scale.linear()
            .domain([0, visMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(ceilingDim)
            .group(ceilGroup)
            .ticks(4)
          .x(d3.scale.linear()
            .domain([0, ceilMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(precipWaterDim)
            .group(pwGroup)
          .x(d3.scale.linear()
            .domain([0, pwMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(aodDim)
            .group(aodGroup)
            .ticks(4)
          .x(d3.scale.linear()
            .domain([0, aodMax])
            .rangeRound([0, 10 * 45]))
          .tickFormat(function(v) { return Number(v).toPrecision(2); }),

        epwBarChart()
            .dimension(snowDim)
            .group(snowGroup)
          .x(d3.scale.linear()
            .domain([0, snowMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(dslsDim)
            .group(dslsGroup)
          .x(d3.scale.linear()
            .domain([0, dslsMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(albedoDim)
            .group(albGroup)
          .x(d3.scale.linear()
            .domain([0, albMax])
            .rangeRound([0, 10 * 45]))
          .tickFormat(function(v) { return Number(v).toPrecision(2); }),

        epwBarChart()
            .dimension(lpdDim)
            .group(lpdGroup)
          .x(d3.scale.linear()
            .domain([0, lpdMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(lpqDim)
            .group(lpqGroup)
          .x(d3.scale.linear()
            .domain([0, lpqMax])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(presDim)
            .group(presGroup)
          .x(d3.scale.linear()
            .domain([presLowX, presHighX])
            .rangeRound([0, 10 * 45])),

        epwBarChart()
            .dimension(pvAcDim)
            .group(pvAcGroup)
          .x(d3.scale.linear()
            .domain([0, pvAcMax])
            .rangeRound([0, 10 * 45]))
          .tickFormat(function(v) { return Number(v).toPrecision(3); })
      ];

    var chart = d3.selectAll(".xfilterChart")
      .data(charts)
      .each(function(chart) { chart.on("brush", renderAll).on("brushend", renderAll); });

    // Render the total.
    d3.selectAll("#xfilterTotal")
        .text(formatNumber(xfilter.size()));

    renderAll();
    window._xfilterRenderAll = renderAll;

    // Renders the specified chart or list.
    function render(method) {
      d3.select(this).call(method);
    }

    // Whenever the brush moves, re-rendering everything.
    function renderAll() {
      chart.each(render);
      d3.select("#xfilterActive").text(formatNumber(all.value()));
      d3.select("#xfilterPercent").text(formatPercent(all.value()/xfilter.size()));
      if (window.updatePsychroPoints) {
        window.updatePsychroPoints(outputDim.top(Infinity));
      }
      if (window.updateWindRose) {
        window.updateWindRose(outputDim.top(Infinity));
      }
      if (window.updateTimeseries) {
        window.updateTimeseries(outputDim.top(Infinity));
      }
      if (window.updateHeatmaps) {
        window.updateHeatmaps(outputDim.top(Infinity));
      }
      if (window.updateTabular) {
        window.updateTabular(outputDim.top(Infinity));
      }
      if (window.updateScatter) {
        window.updateScatter(outputDim.top(Infinity));
      }
      if (window.updateDurationCurve) {
        window.updateDurationCurve(outputDim.top(Infinity));
      }
      if (window.updateDegreeDays) {
        window.updateDegreeDays(outputDim.top(Infinity));
      }
    }

    window.filter = function(filters) {
      filters.forEach(function(d, i) { charts[i].filter(d); });
      renderAll();
    };

    window.reset = function(i) {
      charts[i].filter(null);
      renderAll();
    };

    window.resetAll = function() {
      charts.forEach(function(c) { c.filter(null); });
      renderAll();
    };

    //set up examples
    tempFilterExamples = 'Filter the temperature profile by <a href="javascript:filter([null,[7,19],null])">day</a>, <a href="javascript:filter([[6,9],null,null])">summer months</a>, <a href="javascript:filter([[6,9],[7,19],null])">summer daytime</a>, or ';
    if (unitSystem == "IP") {
        tempFilterExamples = tempFilterExamples + '<a href="javascript:filter([null,null,[65,75]])">hours between 65&degF and 75&degF</a>.'
    } else {
        tempFilterExamples = tempFilterExamples + '<a href="javascript:filter([null,null,[15,25]])">hours between 15&degC and 25&degC</a>.'
    };
    document.getElementById("tempFilterExamples").innerHTML = tempFilterExamples;

    // ---- Variable filter checkbox dropdown ----
    var vfVarList = [
        { divId: 'drybulb-temperature-chart',  chartIdx: 2,  label: 'Dry Bulb Temperature',       defaultOn: true  },
        { divId: 'dewpoint-temperature-chart', chartIdx: 3,  label: 'Dew Point Temperature',      defaultOn: true  },
        { divId: 'rh-chart',                   chartIdx: 4,  label: 'Relative Humidity',           defaultOn: false },
        { divId: 'wetbulb-temperature-chart',  chartIdx: 5,  label: 'Wet Bulb Temperature',        defaultOn: true  },
        { divId: 'enthalpy-chart',             chartIdx: 6,  label: 'Enthalpy',                    defaultOn: false },
        { divId: 'humidity-ratio-chart',       chartIdx: 7,  label: 'Humidity Ratio',              defaultOn: false },
        { divId: 'ghi-chart',                  chartIdx: 8,  label: 'Global Horiz. Radiation',     defaultOn: false },
        { divId: 'dni-chart',                  chartIdx: 9,  label: 'Direct Normal Radiation',     defaultOn: false },
        { divId: 'dhi-chart',                  chartIdx: 10, label: 'Diffuse Horiz. Radiation',    defaultOn: false },
        { divId: 'wind-speed-chart',           chartIdx: 11, label: 'Wind Speed',                  defaultOn: false },
        { divId: 'wind-direction-chart',              chartIdx: 12, label: 'Wind Direction',                defaultOn: false },
        { divId: 'sky-cover-chart',                   chartIdx: 13, label: 'Total Sky Cover',              defaultOn: false },
        { divId: 'opaque-sky-cover-chart',            chartIdx: 14, label: 'Opaque Sky Cover',             defaultOn: false },
        { divId: 'visibility-chart',                  chartIdx: 15, label: 'Visibility',                   defaultOn: false },
        { divId: 'ceiling-height-chart',              chartIdx: 16, label: 'Ceiling Height',               defaultOn: false },
        { divId: 'precipitable-water-chart',          chartIdx: 17, label: 'Precipitable Water',           defaultOn: false },
        { divId: 'aerosol-optical-depth-chart',       chartIdx: 18, label: 'Aerosol Optical Depth',        defaultOn: false },
        { divId: 'snow-depth-chart',                  chartIdx: 19, label: 'Snow Depth',                   defaultOn: false },
        { divId: 'days-since-last-snowfall-chart',    chartIdx: 20, label: 'Days Since Last Snowfall',     defaultOn: false },
        { divId: 'albedo-chart',                      chartIdx: 21, label: 'Albedo',                       defaultOn: false },
        { divId: 'liquid-precip-depth-chart',         chartIdx: 22, label: 'Liquid Precip. Depth',         defaultOn: false },
        { divId: 'liquid-precip-qty-chart',           chartIdx: 23, label: 'Liquid Precip. Quantity',      defaultOn: false },
        { divId: 'pressure-chart',                    chartIdx: 24, label: 'Station Pressure',             defaultOn: false },
        { divId: 'pv-ac-chart',                       chartIdx: 25, label: 'PV AC Output',                 defaultOn: false }
    ];

    // Initialise persistent selection state (preserves across IP/SI re-renders)
    if (!window._vfSelectedVars) {
        window._vfSelectedVars = {};
        vfVarList.forEach(function(v) { window._vfSelectedVars[v.divId] = v.defaultOn; });
    }
    // Ensure any newly-added filter entries are present
    vfVarList.forEach(function(v) {
        if (!(v.divId in window._vfSelectedVars)) {
            window._vfSelectedVars[v.divId] = v.defaultOn;
        }
    });

    // Apply current visibility immediately
    vfVarList.forEach(function(v) {
        var el = document.getElementById(v.divId);
        if (el) el.style.display = window._vfSelectedVars[v.divId] ? '' : 'none';
    });

    // Populate checkbox menu; clone to strip stale listeners
    var vfMenuEl = document.getElementById('vfDropdownMenu');
    if (vfMenuEl) {
        var vfMenuClone = vfMenuEl.cloneNode(false);
        vfMenuEl.parentNode.replaceChild(vfMenuClone, vfMenuEl);
        vfMenuEl = vfMenuClone;
        vfMenuEl.innerHTML = '';

        vfVarList.forEach(function(vd) {
            var lbl = document.createElement('label');
            lbl.className = 'hm-checkbox-item';
            var cb = document.createElement('input');
            cb.type    = 'checkbox';
            cb.value   = vd.divId;
            cb.checked = !!window._vfSelectedVars[vd.divId];
            lbl.appendChild(cb);
            lbl.appendChild(document.createTextNode('\u00A0' + vd.label));
            vfMenuEl.appendChild(lbl);

            cb.addEventListener('change', function() {
                var on = this.checked;
                window._vfSelectedVars[vd.divId] = on;
                var el = document.getElementById(vd.divId);
                if (el) el.style.display = on ? '' : 'none';
                // Clear the crossfilter dimension when hiding so it doesn't silently filter
                if (!on) {
                    charts[vd.chartIdx].filter(null);
                }
                renderAll();
            });
        });
    }

    // Toggle button
    var vfToggleBtn = document.getElementById('vfDropdownToggle');
    if (vfToggleBtn) {
        vfToggleBtn.onclick = function(e) {
            e.stopPropagation();
            if (vfMenuEl) vfMenuEl.classList.toggle('open');
        };
    }

    // Outside-click closes the dropdown
    if (window._vfDocClickHandler) {
        document.removeEventListener('click', window._vfDocClickHandler);
    }
    window._vfDocClickHandler = function(e) {
        var dd = document.getElementById('vfDropdown');
        if (dd && !dd.contains(e.target) && vfMenuEl) {
            vfMenuEl.classList.remove('open');
        }
    };
    document.addEventListener('click', window._vfDocClickHandler);

    // Expose rebuild hook so epwSolarPV can refresh crossfilter after Recalculate
    window._pvRebuildValueFilter = function() { epwValueFilter(epw); };
};

//general code for making a bar chart with crossfilter
function epwBarChart() {
    var margin = {top: 10, right: 10, bottom: 20, left: 10},
        x,
        y = d3.scale.linear().range([65, 0]),
        id = window._epwChartIdCounter++,
        axis = d3.svg.axis().orient("bottom"),
        brush = d3.svg.brush(),
        brushDirty,
        dimension,
        group,
        round,
        _inputMin = null,
        _inputMax = null;

    function fmtVal(v) {
        v = +v;
        return isFinite(v) ? (v === Math.round(v) ? String(v) : v.toFixed(2)) : '';
    }

    function chart(div) {
      var width = x.range()[1],
          height = y.range()[0];

      y.domain([0, group.top(1)[0].value]);

      div.each(function() {
        var div = d3.select(this),
            g = div.select("g");

        // Create the skeletal chart.
        if (g.empty()) {
          div.select(".title").append("a")
              .attr("href", "javascript:reset(" + id + ")")
              .attr("class", "reset")
              .text("reset")
              .style("display", "none");

          g = div.append("svg")
              .attr("width", width + margin.left + margin.right)
              .attr("height", height + margin.top + margin.bottom)
            .append("g")
              .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

          g.append("clipPath")
              .attr("id", "clip-" + id)
            .append("rect")
              .attr("width", width)
              .attr("height", height);

          g.selectAll(".bar")
              .data(["background", "foreground"])
            .enter().append("path")
              .attr("class", function(d) { return d + " bar"; })
              .datum(group.all());

          g.selectAll(".foreground.bar")
              .attr("clip-path", "url(#clip-" + id + ")");

          g.append("g")
              .attr("class", "axis")
              .attr("transform", "translate(0," + height + ")")
              .call(axis);

          // Initialize the brush component with pretty resize handles.
          var gBrush = g.append("g").attr("class", "brush").call(brush);
          gBrush.selectAll("rect").attr("height", height);
          gBrush.selectAll(".resize").append("path").attr("d", resizePath);
          // Inject min/max input group into the title row (first render only)
          var _tr0 = div.select('.chart-title-row');
          if (!_tr0.empty() && _tr0.select('.vf-range-inputs').empty()) {
              var _rd = document.createElement('div');
              _rd.className = 'vf-range-inputs';
              _rd.innerHTML = 'min:\u00A0<input class="vf-min-input" type="text" placeholder="\u2013"> max:\u00A0<input class="vf-max-input" type="text" placeholder="\u2013">';
              var _dl = _tr0.select('.chart-download-btns').node();
              _tr0.node().insertBefore(_rd, _dl || null);
          }
        }

        // Resolve input refs and attach handlers once per chart instance (handles IP/SI re-creates)
        if (!_inputMin) {
            var _tr = div.select('.chart-title-row');
            if (!_tr.empty()) {
                var _existMin = _tr.select('.vf-min-input').node();
                if (_existMin) {
                    // Clone to strip any stale listeners from a previous chart instance
                    var _newMin = _existMin.cloneNode(true);
                    _existMin.parentNode.replaceChild(_newMin, _existMin);
                    var _existMax = _tr.select('.vf-max-input').node();
                    var _newMax = _existMax.cloneNode(true);
                    _existMax.parentNode.replaceChild(_newMax, _existMax);
                    _inputMin = _newMin;
                    _inputMax = _newMax;
                    _inputMin.value = '';
                    _inputMax.value = '';
                    (function(minIn, maxIn, chartRef) {
                        function _applyFn() {
                            var mn = parseFloat(minIn.value);
                            var mx = parseFloat(maxIn.value);
                            if (isNaN(mn) && isNaN(mx)) {
                                chartRef.filter(null);
                            } else {
                                var dom = x.domain();
                                chartRef.filter([isNaN(mn) ? dom[0] : mn, isNaN(mx) ? dom[1] : mx]);
                            }
                            if (window._xfilterRenderAll) window._xfilterRenderAll();
                        }
                        minIn.addEventListener('change', _applyFn);
                        maxIn.addEventListener('change', _applyFn);
                        minIn.addEventListener('keydown', function(e) { if (e.key === 'Enter') _applyFn(); });
                        maxIn.addEventListener('keydown', function(e) { if (e.key === 'Enter') _applyFn(); });
                    }(_inputMin, _inputMax, chart));
                }
            }
        }

        // Only redraw the brush if set externally.
        if (brushDirty) {
          brushDirty = false;
          g.selectAll(".brush").call(brush);
          div.select(".title a").style("display", brush.empty() ? "none" : null);
          if (brush.empty()) {
            g.selectAll("#clip-" + id + " rect")
                .attr("x", 0)
                .attr("width", width);
          } else {
            var extent = brush.extent();
            g.selectAll("#clip-" + id + " rect")
                .attr("x", x(extent[0]))
                .attr("width", x(extent[1]) - x(extent[0]));
          }
        }

        g.selectAll(".bar").attr("d", barPath);
      });

      function barPath(groups) {
        var path = [],
            i = -1,
            n = groups.length,
            d;
        while (++i < n) {
          d = groups[i];
          path.push("M", x(d.key), ",", height, "V", y(d.value), "h9V", height);
        }
        return path.join("");
      };

      function resizePath(d) {
        var e = +(d == "e"),
            x = e ? 1 : -1,
            y = height / 3;
        return "M" + (.5 * x) + "," + y
            + "A6,6 0 0 " + e + " " + (6.5 * x) + "," + (y + 6)
            + "V" + (2 * y - 6)
            + "A6,6 0 0 " + e + " " + (.5 * x) + "," + (2 * y)
            + "Z"
            + "M" + (2.5 * x) + "," + (y + 8)
            + "V" + (2 * y - 8)
            + "M" + (4.5 * x) + "," + (y + 8)
            + "V" + (2 * y - 8);
      };
    }

    brush.on("brushstart.chart", function() {
      var div = d3.select(this.parentNode.parentNode.parentNode);
      div.select(".title a").style("display", null);
    });

    brush.on("brush.chart", function() {
      var g = d3.select(this.parentNode),
          extent = brush.extent();
      if (round) g.select(".brush")
          .call(brush.extent(extent = extent.map(round)))
        .selectAll(".resize")
          .style("display", null);
      g.select("#clip-" + id + " rect")
          .attr("x", x(extent[0]))
          .attr("width", x(extent[1]) - x(extent[0]));
      dimension.filterRange(extent);
      if (_inputMin) _inputMin.value = fmtVal(extent[0]);
      if (_inputMax) _inputMax.value = fmtVal(extent[1]);
    });

    brush.on("brushend.chart", function() {
      if (brush.empty()) {
        var div = d3.select(this.parentNode.parentNode.parentNode);
        div.select(".title a").style("display", "none");
        div.select("#clip-" + id + " rect").attr("x", null).attr("width", "100%");
        dimension.filterAll();
        if (_inputMin) { _inputMin.value = ''; _inputMax.value = ''; }
      }
    });

    chart.margin = function(_) {
      if (!arguments.length) return margin;
      margin = _;
      return chart;
    };

    chart.x = function(_) {
      if (!arguments.length) return x;
      x = _;
      axis.scale(x);
      brush.x(x);
      return chart;
    };

    chart.tickFormat = function(_) {
      if (!arguments.length) return axis.tickFormat();
      axis.tickFormat(_);
      return chart;
    };

    chart.ticks = function(_) {
      if (!arguments.length) return axis.ticks();
      axis.ticks(_);
      return chart;
    };

    chart.y = function(_) {
      if (!arguments.length) return y;
      y = _;
      return chart;
    };

    chart.dimension = function(_) {
      if (!arguments.length) return dimension;
      dimension = _;
      return chart;
    };

    chart.filter = function(_) {
      if (_) {
        brush.extent(_);
        dimension.filterRange(_);
        if (_inputMin) _inputMin.value = fmtVal(_[0]);
        if (_inputMax) _inputMax.value = fmtVal(_[1]);
      } else {
        brush.clear();
        dimension.filterAll();
        if (_inputMin) { _inputMin.value = ''; _inputMax.value = ''; }
      }
      brushDirty = true;
      return chart;
    };

    chart.group = function(_) {
      if (!arguments.length) return group;
      group = _;
      return chart;
    };

    chart.round = function(_) {
      if (!arguments.length) return round;
      round = _;
      return chart;
    };

    return d3.rebind(chart, brush, "on");
};

// Polar (circular) bar chart for cyclic dimensions (month, hour) with wrap-around brush selection.
// Drag clockwise from any starting segment to brush a contiguous arc; dragging past 12 o'clock
// wraps around so e.g. winter months (Nov–Jan) or night hours (10pm–2am) can be selected.
function epwPolarChart() {
    var id = window._epwChartIdCounter++,
        dimension,
        group,
        labels        = [],
        outerRadius   = 80,
        innerRadius   = 24,
        labelOffset   = 0.5,    // 0 = segment start, 0.5 = segment center
        filterExtent  = null,   // [startSeg, endSeg] 0-based, or null
        brushDirty      = false,
        showRangeInputs = true,
        _inputMin       = null,
        _inputMax       = null,
        dispatch        = d3.dispatch("brush", "brushend");

    function chart(div) {
        div.each(function() {
            var container = d3.select(this);
            var groupData = group.all();
            var n         = groupData.length;
            var maxVal    = d3.max(groupData, function(d) { return d.value; }) || 1;
            var angleStep = (2 * Math.PI) / n;
            var rScale    = d3.scale.linear().domain([0, maxVal]).range([innerRadius, outerRadius]);

            var svgW = (outerRadius + 22) * 2;
            var svgH = (outerRadius + 22) * 2;
            var cx = svgW / 2, cy = svgH / 2;

            var svg = container.select("svg");

            if (svg.empty()) {
                container.select(".title").append("a")
                    .attr("href", "javascript:reset(" + id + ")")
                    .attr("class", "reset")
                    .text("reset")
                    .style("display", "none");

                // Inject min/max inputs into title row (once per chart div lifetime)
                var _tr0 = container.select('.chart-title-row');
                if (showRangeInputs && !_tr0.empty() && _tr0.select('.vf-range-inputs').empty()) {
                    var _rd = document.createElement('div');
                    _rd.className = 'vf-range-inputs';
                    _rd.innerHTML = 'min:\u00A0<input class="vf-min-input" type="text" placeholder="\u2013"> max:\u00A0<input class="vf-max-input" type="text" placeholder="\u2013">';
                    var _dl = _tr0.select('.chart-download-btns').node();
                    _tr0.node().insertBefore(_rd, _dl || null);
                }

                svg = container.append("svg")
                    .attr("width",  svgW)
                    .attr("height", svgH)
                    .style("cursor", "crosshair");

                var g = svg.append("g")
                    .attr("class", "polar-g")
                    .attr("transform", "translate(" + cx + "," + cy + ")");

                g.append("g").attr("class", "polar-bars-bg");
                g.append("path").attr("class", "polar-selection");
                g.append("g").attr("class", "polar-bars-fg");
                g.append("g").attr("class", "polar-labels");

                var dragState = { active: false, prevAngle: 0, sweepAngle: 0, startSeg: 0 };

                function rawAngle(node) {
                    var m = d3.mouse(node);
                    var a = Math.atan2(m[0] - cx, -(m[1] - cy));
                    return a < 0 ? a + 2 * Math.PI : a;
                }

                function angleDelta(prev, curr) {
                    var d = curr - prev;
                    if (d >  Math.PI) d -= 2 * Math.PI;
                    if (d < -Math.PI) d += 2 * Math.PI;
                    return d;
                }

                svg.on("mousedown", function() {
                    var a = rawAngle(svg.node());
                    dragState.active     = true;
                    dragState.prevAngle  = a;
                    dragState.sweepAngle = 0;
                    dragState.startSeg   = Math.floor(a / angleStep) % n;
                    d3.event.preventDefault();
                    redrawViz(container, svg, g, n, groupData, rScale, angleStep, dragState.startSeg, 0);
                });

                svg.on("mousemove", function() {
                    if (!dragState.active) return;
                    var a = rawAngle(svg.node());
                    var delta = angleDelta(dragState.prevAngle, a);
                    dragState.sweepAngle = Math.max(0, Math.min(2 * Math.PI - angleStep * 0.5, dragState.sweepAngle + delta));
                    dragState.prevAngle  = a;
                    redrawViz(container, svg, g, n, groupData, rScale, angleStep, dragState.startSeg, dragState.sweepAngle);
                    dispatch.brush.call(chart);
                });

                function endDrag() {
                    if (!dragState.active) return;
                    dragState.active = false;
                    if (dragState.sweepAngle < angleStep * 0.5) {
                        // tiny drag / click = clear filter
                        filterExtent = null;
                        dimension.filterAll();
                        container.select(".title a.reset").style("display", "none");
                        redrawViz(container, svg, g, n, groupData, rScale, angleStep, null, 0);
                        if (_inputMin) { _inputMin.value = ''; _inputMax.value = ''; }
                    } else {
                        var numSegs  = Math.round(dragState.sweepAngle / angleStep);
                        var startSeg = dragState.startSeg;
                        var endSeg   = (startSeg + numSegs - 1 + n) % n;
                        filterExtent = [startSeg, endSeg];
                        applyFilter(groupData, n, startSeg, endSeg);
                        container.select(".title a.reset").style("display", null);
                        redrawViz(container, svg, g, n, groupData, rScale, angleStep, startSeg, numSegs * angleStep);
                        if (_inputMin) _inputMin.value = String(groupData[startSeg].key);
                        if (_inputMax) _inputMax.value = String(groupData[endSeg].key);
                    }
                    dispatch.brushend.call(chart);
                }

                svg.on("mouseup",    endDrag);
                svg.on("mouseleave", endDrag);
            }

            // Resolve input refs and attach handlers once per chart instance
            if (showRangeInputs && !_inputMin) {
                var _tr = container.select('.chart-title-row');
                if (!_tr.empty()) {
                    var _existMin = _tr.select('.vf-min-input').node();
                    if (_existMin) {
                        var _newMin = _existMin.cloneNode(true);
                        _existMin.parentNode.replaceChild(_newMin, _existMin);
                        var _existMax = _tr.select('.vf-max-input').node();
                        var _newMax = _existMax.cloneNode(true);
                        _existMax.parentNode.replaceChild(_newMax, _existMax);
                        _inputMin = _newMin;
                        _inputMax = _newMax;
                        _inputMin.value = '';
                        _inputMax.value = '';
                        (function(minIn, maxIn, chartRef) {
                            function _applyFn() {
                                var mn = parseFloat(minIn.value);
                                var mx = parseFloat(maxIn.value);
                                if (isNaN(mn) && isNaN(mx)) {
                                    chartRef.filter(null);
                                } else {
                                    var gd   = group.all();
                                    var keys = gd.map(function(d) { return d.key; });
                                    mn = isNaN(mn) ? keys[0] : mn;
                                    mx = isNaN(mx) ? keys[keys.length - 1] : mx;
                                    chartRef.filter([mn, mx + 1]);
                                }
                                if (window._xfilterRenderAll) window._xfilterRenderAll();
                            }
                            minIn.addEventListener('change',  _applyFn);
                            maxIn.addEventListener('change',  _applyFn);
                            minIn.addEventListener('keydown', function(e) { if (e.key === 'Enter') _applyFn(); });
                            maxIn.addEventListener('keydown', function(e) { if (e.key === 'Enter') _applyFn(); });
                        }(_inputMin, _inputMax, chart));
                    }
                }
            }

            // Redraw bars on every renderAll call (crossfilter changes bar heights)
            var g = svg.select(".polar-g");
            drawBars(g, n, groupData, rScale, angleStep);

            if (brushDirty) {
                brushDirty = false;
                if (filterExtent) {
                    var startSeg  = filterExtent[0];
                    var numSegs   = (filterExtent[1] - filterExtent[0] + n) % n + 1;
                    redrawViz(container, svg, g, n, groupData, rScale, angleStep, startSeg, numSegs * angleStep);
                    container.select(".title a.reset").style("display", null);
                } else {
                    redrawViz(container, svg, g, n, groupData, rScale, angleStep, null, 0);
                    container.select(".title a.reset").style("display", "none");
                }
            }
        });
    }

    function drawBars(g, n, groupData, rScale, angleStep) {
        // In d3 v3 the original selection only covers the UPDATE set after enter().append().
        // Re-select after enter so attributes are applied to newly created elements too.
        var manySegs = n > 36; // suppress per-segment strokes for dense charts (e.g. 360-bin wind dir)

        var bgGroup = g.select(".polar-bars-bg");
        bgGroup.selectAll("path.pbg").data(groupData).enter().append("path").attr("class", "pbg");
        bgGroup.selectAll("path.pbg")
            .attr("d", function(d, i) {
                return sectorPath(i * angleStep, (i + 1) * angleStep, innerRadius, outerRadius);
            })
            .style("fill", "#ccc")
            .style("stroke", manySegs ? "none" : "white")
            .style("stroke-width", manySegs ? "0" : "0.5px");

        var fgGroup = g.select(".polar-bars-fg");
        fgGroup.selectAll("path.pfg").data(groupData).enter().append("path").attr("class", "pfg");
        fgGroup.selectAll("path.pfg")
            .attr("d", function(d, i) {
                return sectorPath(i * angleStep, (i + 1) * angleStep, innerRadius, Math.max(innerRadius, rScale(d.value)));
            })
            .style("fill", function() { return window._plotColor || 'steelblue'; })
            .style("stroke", manySegs ? "none" : "white")
            .style("stroke-width", manySegs ? "0" : "0.5px");

        // For dense charts draw tick lines every 15 segments instead of per-segment strokes
        if (manySegs) {
            var tickGroup = g.select(".polar-ticks");
            if (tickGroup.empty()) tickGroup = g.insert("g", ".polar-labels").attr("class", "polar-ticks");
            var tickStep = Math.round((2 * Math.PI) / 24); // 24 ticks = every 15° for n=360
            var tickAngles = [];
            for (var t = 0; t < n; t++) {
                if (t % Math.round(n / 24) === 0) tickAngles.push(t * angleStep);
            }
            var tickSel = tickGroup.selectAll("line.ptick").data(tickAngles);
            tickSel.enter().append("line").attr("class", "ptick");
            tickSel
                .attr("x1", function(a) { return (innerRadius * Math.sin(a)).toFixed(2); })
                .attr("y1", function(a) { return (-innerRadius * Math.cos(a)).toFixed(2); })
                .attr("x2", function(a) { return (outerRadius * Math.sin(a)).toFixed(2); })
                .attr("y2", function(a) { return (-outerRadius * Math.cos(a)).toFixed(2); })
                .style("stroke", "white")
                .style("stroke-width", "1px");
            tickSel.exit().remove();
        }

        var lblGroup = g.select(".polar-labels");
        lblGroup.selectAll("text.plbl").data(groupData).enter().append("text").attr("class", "plbl");
        lblGroup.selectAll("text.plbl")
            .attr("transform", function(d, i) {
                var angle = (i + labelOffset) * angleStep;
                var r = outerRadius + 13;
                return "translate(" + (r * Math.sin(angle)).toFixed(2) + "," + (-r * Math.cos(angle)).toFixed(2) + ")";
            })
            .attr("text-anchor", "middle")
            .attr("dominant-baseline", "middle")
            .style("font", "9px sans-serif")
            .text(function(d, i) { return labels[i] !== undefined ? labels[i] : d.key; });
    }

    function redrawViz(container, svg, g, n, groupData, rScale, angleStep, startSeg, sweepAngle) {
        drawBars(g, n, groupData, rScale, angleStep);
        var selPath = "";
        if (startSeg !== null && sweepAngle >= angleStep * 0.5) {
            var sa = startSeg * angleStep;
            var ea = sa + sweepAngle;
            selPath = sectorPath(sa, ea, innerRadius, outerRadius + 8);
        }
        g.select(".polar-selection")
            .attr("d", selPath)
            .style("fill", function() { return window._plotColor || 'steelblue'; })
            .style("fill-opacity", 0.2)
            .style("stroke", function() { return window._plotColor || 'steelblue'; })
            .style("stroke-width", "1px");
    }

    function applyFilter(groupData, n, startSeg, endSeg) {
        var keys = groupData.map(function(d) { return d.key; });
        if (startSeg <= endSeg) {
            dimension.filterRange([keys[startSeg], keys[endSeg] + 1]);
        } else {
            // wrap-around: e.g. startSeg=10, endSeg=2 includes keys 10,11,0,1,2
            var minKey = keys[startSeg];
            var maxKey = keys[endSeg];
            dimension.filterFunction(function(d) { return d >= minKey || d <= maxKey; });
        }
    }

    // Arc sector path: angles measured clockwise from 12 o'clock (north)
    function sectorPath(startAngle, endAngle, inner, outer) {
        var largeArc = (endAngle - startAngle) > Math.PI ? 1 : 0;
        var sinS = Math.sin(startAngle), cosS = Math.cos(startAngle);
        var sinE = Math.sin(endAngle),   cosE = Math.cos(endAngle);
        return ["M", (outer*sinS).toFixed(3), (-outer*cosS).toFixed(3),
                "A", outer, outer, 0, largeArc, 1, (outer*sinE).toFixed(3), (-outer*cosE).toFixed(3),
                "L", (inner*sinE).toFixed(3), (-inner*cosE).toFixed(3),
                "A", inner, inner, 0, largeArc, 0, (inner*sinS).toFixed(3), (-inner*cosS).toFixed(3),
                "Z"].join(" ");
    }

    chart.dimension = function(_) {
        if (!arguments.length) return dimension;
        dimension = _;
        return chart;
    };

    chart.group = function(_) {
        if (!arguments.length) return group;
        group = _;
        return chart;
    };

    chart.labels = function(_) {
        if (!arguments.length) return labels;
        labels = _;
        return chart;
    };

    chart.radius = function(_) {
        if (!arguments.length) return outerRadius;
        outerRadius = _;
        return chart;
    };

    chart.innerRadius = function(_) {
        if (!arguments.length) return innerRadius;
        innerRadius = _;
        return chart;
    };

    chart.labelOffset = function(_) {
        if (!arguments.length) return labelOffset;
        labelOffset = _;
        return chart;
    };

    chart.showRangeInputs = function(_) {
        if (!arguments.length) return showRangeInputs;
        showRangeInputs = !!_;
        return chart;
    };

    chart.filter = function(_) {
        if (_) {
            // _ = [minKey, maxKey+1] as used by window.filter() / window.reset()
            var groupData = group.all();
            var keys = groupData.map(function(d) { return d.key; });
            var n = keys.length;
            var startSeg = -1, endSeg = -1;
            for (var i = 0; i < n; i++) {
                if (keys[i] >= _[0] && startSeg === -1) startSeg = i;
                if (keys[i] < _[1]) endSeg = i;
            }
            if (startSeg === -1) startSeg = 0;
            if (endSeg   === -1) endSeg   = n - 1;
            filterExtent = [startSeg, endSeg];
            dimension.filterRange(_);
            if (_inputMin) _inputMin.value = String(_[0]);
            if (_inputMax) _inputMax.value = String(_[1] - 1);
        } else {
            filterExtent = null;
            dimension.filterAll();
            if (_inputMin) { _inputMin.value = ''; _inputMax.value = ''; }
        }
        brushDirty = true;
        return chart;
    };

    return d3.rebind(chart, dispatch, "on");
}

//initialization code for the psychrometric chart
function epwPsychroChart(epw) {
    // psychrolib calculations always run in SI
    psychrolib.SetUnitSystem(psychrolib.SI);

    // Use the average station pressure from the EPW data so RH curves align
    // with data points (which were also computed at actual station pressure).
    // EPW field 9 = Atmospheric Station Pressure (Pa).
    var pressureData = epw.getDataByField(9).filter(function(p) { return isFinite(p) && p > 0; });
    var ATM = pressureData.length > 0
        ? pressureData.reduce(function(a, b) { return a + b; }, 0) / pressureData.length
        : 101325;

    var isIP = (unitSystem === "IP");

    // Helpers: convert between SI and display units
    function dbToDisp(tC)   { return isIP ? (32 + tC * 9/5) : tC; }
    function dbToSI(tDisp)  { return isIP ? ((tDisp - 32) * 5/9) : tDisp; }
    function hrToDisp(hrKg) { return isIP ? hrKg * 7000 : hrKg * 1000; }

    // Build display data using epwDataDefault (same source as crossfilter)
    // dryBulbTemperature is already in display units; humidityRatio is always kg/kg
    var rawData = epwDataDefault(epw, unitSystem);
    var displayData = [];
    rawData.forEach(function(d) {
        if (d.humidityRatio !== null && isFinite(d.humidityRatio) &&
            d.dryBulbTemperature !== null && isFinite(d.dryBulbTemperature)) {
            displayData.push({
                index:  d.index,
                db:     d.dryBulbTemperature,
                hr:     hrToDisp(d.humidityRatio),
                rh:     d.relativeHumidity,
                month:  d.month,
                day:    d.day,
                hour:   d.hour
            });
        }
    });
    if (displayData.length === 0) return;

    // Axis ranges
    var xPad  = isIP ? 5 : 2;
    var dbMin = d3.min(displayData, function(d) { return d.db; }) - xPad;
    var dbMax = d3.max(displayData, function(d) { return d.db; }) + xPad;
    var hrMax = d3.max(displayData, function(d) { return d.hr; }) * 1.1;

    // Chart geometry
    var margin = {top: 20, right: 130, bottom: 50, left: 70};
    var width  = 720;
    var height = 450;

    var x = d3.scale.linear().domain([dbMin, dbMax]).range([0, width]);
    var y = d3.scale.linear().domain([0, hrMax]).range([height, 0]);

    var svg = d3.select("#epwPsychroChart").append("svg")
        .attr("width",  width  + margin.left + margin.right)
        .attr("height", height + margin.top  + margin.bottom)
        .append("g")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    // --- Saturation curve (100% RH) ------------------------------------
    var nSteps    = 300;
    var tStep     = (dbMax - dbMin) / nSteps;
    var satCurve  = [];
    for (var t = dbMin; t <= dbMax + tStep / 2; t += tStep) {
        try {
            var hrKg = psychrolib.GetHumRatioFromRelHum(dbToSI(t), 1.0, ATM);
            var hrD  = hrToDisp(hrKg);
            satCurve.push({db: t, hr: hrD});
        } catch(e) {}
    }

    // Build SVG clip-path: region below the saturation curve
    // Saturation curve runs low-left → high-right in screen y (it rises from bottom to top).
    var clipId = "psychro-clip-" + Math.floor(Math.random() * 1e6);
    (function() {
        var pts = satCurve.map(function(d) { return [x(d.db), y(d.hr)]; });
        if (pts.length < 2) return;

        var clipD = "M" + pts[0][0] + "," + pts[0][1];
        for (var i = 1; i < pts.length; i++) {
            clipD += " L" + pts[i][0] + "," + pts[i][1];
        }
        // close: down-right corner → down-left corner → back to start
        clipD += " L" + width  + "," + height;
        clipD += " L0,"         + height;
        clipD += " L0,"         + pts[0][1];
        clipD += " Z";

        svg.append("defs").append("clipPath")
            .attr("id", clipId)
            .append("path")
            .attr("d", clipD);
    }());

    // --- Grid lines (clipped so they terminate at the saturation curve) --
    var gridGroup = svg.append("g")
        .attr("class", "psychro-grid")
        .attr("clip-path", "url(#" + clipId + ")");

    // Vertical lines (constant temperature)
    x.ticks(10).forEach(function(tv) {
        gridGroup.append("line")
            .attr("x1", x(tv)).attr("x2", x(tv))
            .attr("y1", 0)    .attr("y2", height)
            .attr("stroke", "lightgray").attr("stroke-width", 0.5);
    });

    // Horizontal lines (constant humidity ratio)
    y.ticks(8).forEach(function(hv) {
        gridGroup.append("line")
            .attr("x1", 0)    .attr("x2", width)
            .attr("y1", y(hv)).attr("y2", y(hv))
            .attr("stroke", "lightgray").attr("stroke-width", 0.5);
    });

    // --- Binned heatmap --------------------------------------------------
    var gridCols = 60, gridRows = 45;
    var binW = (dbMax - dbMin) / gridCols;
    var binH = hrMax / gridRows;
    var cellPxW = Math.ceil(width  / gridCols) + 1;
    var cellPxH = Math.ceil(height / gridRows) + 1;

    var _hmC = getHmColors();
    var _hmD = d3.range(_hmC.length).map(function(i) { return i / (_hmC.length - 1); });
    var heatColorScale = d3.scale.linear().domain(_hmD).range(_hmC);

    // --- Legend variables (DOM elements created after heatmap group) -----
    var legendStops   = [0, 0.14, 0.29, 0.43, 0.57, 0.71, 0.86, 1];
    var legendSwatchH = 14, legendSwatchW = 12, legendGap = 2;
    var legendX = 10;
    var legendY = 10;
    var legendLabels  = [];   // populated below, after heatmap group

    function updateLegendLabels(maxCount) {
        if (!legendLabels.length) return;   // DOM not yet created
        // Compute log-spaced target values, then enforce strict ascending
        // uniqueness so no two swatches ever show the same number.
        var vals = legendStops.map(function(norm) {
            return Math.round(Math.exp(norm * Math.log(maxCount + 1)) - 1);
        });
        vals[0] = Math.max(1, vals[0]);
        for (var i = 1; i < vals.length; i++) {
            vals[i] = Math.max(vals[i - 1] + 1, vals[i]);
        }
        vals.forEach(function(v, i) {
            // Blank the label for swatches that exceed the actual max
            legendLabels[i].text(v > maxCount ? "" : String(v));
        });
    }

    var heatGroup = svg.append("g")
        .attr("class", "psychro-heatmap")
        .attr("clip-path", "url(#" + clipId + ")");

    // Pre-build one rect per cell
    var heatCells = [];
    for (var hrow = 0; hrow < gridRows; hrow++) {
        for (var hcol = 0; hcol < gridCols; hcol++) {
            var hrect = heatGroup.append("rect")
                .attr("x",      x(dbMin + hcol * binW))
                .attr("y",      y((hrow + 1) * binH))
                .attr("width",  cellPxW)
                .attr("height", cellPxH)
                .attr("fill",   "none");
            heatCells.push({ rect: hrect, row: hrow, col: hcol });
        }
    }

    function drawHeatmap(activeData) {
        var counts = {};
        var maxCount = 0;
        activeData.forEach(function(d) {
            var col = Math.floor((d.db - dbMin) / binW);
            var row = Math.floor(d.hr / binH);
            col = Math.max(0, Math.min(gridCols - 1, col));
            row = Math.max(0, Math.min(gridRows - 1, row));
            var key = row * gridCols + col;
            counts[key] = (counts[key] || 0) + 1;
            if (counts[key] > maxCount) maxCount = counts[key];
        });
        heatCells.forEach(function(c) {
            var cnt = counts[c.row * gridCols + c.col] || 0;
            c.count = cnt; // store for tooltip
            if (cnt === 0) {
                c.rect.attr("fill", "none");
            } else {
                var norm = Math.log(cnt + 1) / Math.log(maxCount + 1);
                c.rect.attr("fill", heatColorScale(norm)).attr("opacity", 1.0);
            }
        });
        updateLegendLabels(maxCount);
    }

    drawHeatmap(displayData);

    // --- Tooltip events on psychochart heatmap cells ---------------------
    heatCells.forEach(function(c) {
        c.rect
            .on('mouseover', function() {
                if (!c.count) return;
                var tt = document.getElementById('epw-psychro-tooltip');
                if (!tt) return;
                var tUnit = isIP ? '\u00B0F' : '\u00B0C';
                var hUnit = isIP ? 'gr/lb' : 'g/kg';
                var dbLo  = (dbMin + c.col * binW).toFixed(1);
                var dbHi  = (dbMin + (c.col + 1) * binW).toFixed(1);
                var hrDec = isIP ? 0 : 2;
                var hrLo  = (c.row * binH).toFixed(hrDec);
                var hrHi  = ((c.row + 1) * binH).toFixed(hrDec);
                tt.innerHTML = 'DB: ' + dbLo + '\u2013' + dbHi + '\u00A0' + tUnit
                    + '<br>HR: ' + hrLo + '\u2013' + hrHi + '\u00A0' + hUnit
                    + '<br><strong>' + c.count + '\u00A0hr' + (c.count !== 1 ? 's' : '') + '</strong>';
                tt.style.display = 'block';
                tt.style.left = (d3.event.clientX + 12) + 'px';
                tt.style.top  = (d3.event.clientY + 14) + 'px';
            })
            .on('mousemove', function() {
                var tt = document.getElementById('epw-psychro-tooltip');
                if (!tt) return;
                tt.style.left = (d3.event.clientX + 12) + 'px';
                tt.style.top  = (d3.event.clientY + 14) + 'px';
            })
            .on('mouseout', function() {
                var tt = document.getElementById('epw-psychro-tooltip');
                if (tt) tt.style.display = 'none';
            });
    });

    // --- Legend DOM (appended after heatmap so it renders on top) --------
    var legendTotalH = legendStops.length * (legendSwatchH + legendGap) + 16;
    var legendGroup = svg.append("g").attr("class", "psychro-legend");

    legendGroup.append("rect")
        .attr("x",      legendX - 4)
        .attr("y",      legendY - 4)
        .attr("width",  legendSwatchW + 42)
        .attr("height", legendTotalH)
        .attr("fill",   "white")
        .attr("opacity", 0.75)
        .attr("rx", 3);

    legendGroup.append("text")
        .attr("x", legendX)
        .attr("y", legendY + 8)
        .attr("font-size", "10px")
        .attr("fill", "#555")
        .text("hrs/bin");

    legendStops.forEach(function(norm, i) {
        legendGroup.append("rect")
            .attr("x",      legendX)
            .attr("y",      legendY + 14 + i * (legendSwatchH + legendGap))
            .attr("width",  legendSwatchW)
            .attr("height", legendSwatchH)
            .attr("fill",   heatColorScale(norm));
        legendLabels[i] = legendGroup.append("text")
            .attr("x", legendX + legendSwatchW + 4)
            .attr("y", legendY + 14 + i * (legendSwatchH + legendGap) + legendSwatchH - 3)
            .attr("font-size", "10px")
            .attr("fill", "#555");
    });
    drawHeatmap(displayData); // second pass: legend DOM now exists, fills labels

    // --- RH curves -------------------------------------------------------
    var rhValues = [10, 20, 30, 40, 50, 60, 70, 80, 90, 100];
    var lineFn = d3.svg.line()
        .x(function(d) { return x(d.db); })
        .y(function(d) { return y(d.hr); })
        .defined(function(d) {
            return d.hr !== null && isFinite(d.hr) && d.hr >= 0 && d.hr <= hrMax * 1.05;
        });

    var rhGroup = svg.append("g").attr("class", "psychro-rh-lines");

    rhValues.forEach(function(rh, rhIndex) {
        var pts = [];
        for (var t = dbMin; t <= dbMax + tStep / 2; t += tStep) {
            var hrKg = null;
            try { hrKg = psychrolib.GetHumRatioFromRelHum(dbToSI(t), rh / 100, ATM); }
            catch(e) {}
            pts.push({ db: t, hr: hrKg !== null ? hrToDisp(hrKg) : null });
        }

        rhGroup.append("path")
            .datum(pts)
            .attr("d", lineFn)
            .attr("fill", "none")
            .attr("stroke", "lightgray")
            .attr("stroke-width", rh === 100 ? 1.5 : 0.8);

        // Label: centered over the rightmost valid point, staggered every other line
        var labelPt = null;
        for (var j = pts.length - 1; j >= 0; j--) {
            if (pts[j].hr !== null && pts[j].hr >= 0 && pts[j].hr <= hrMax) {
                labelPt = pts[j];
                break;
            }
        }
        if (labelPt) {
            // No offset for low RH; stagger + drop 40–100% to avoid clipping at chart top
            var yOffset = 0;
            if (rh >= 40) { yOffset = (rhIndex % 2 === 0) ? -6 + 12 : -24 + 12; }
            rhGroup.append("text")
                .attr("x", x(labelPt.db))
                .attr("y", y(labelPt.hr) + yOffset)
                .attr("text-anchor", "middle")
                .attr("font-size", "9px")
                .attr("fill", "#aaa")
                .text(rh + "% RH");
        }
    });

    // --- Axes ------------------------------------------------------------
    var xAxis = d3.svg.axis().scale(x).orient("bottom").ticks(10);
    var yAxis = d3.svg.axis().scale(y).orient("right").ticks(8);

    svg.append("g")
        .attr("class", "axis")
        .attr("transform", "translate(0," + height + ")")
        .call(xAxis);

    svg.append("g")
        .attr("class", "axis")
        .attr("transform", "translate(" + width + ",0)")
        .call(yAxis);

    // --- Dewpoint temperature secondary y-axis (left) --------------------
    // Ticks share the same pixel positions as the humidity ratio axis.
    // For each HR tick, compute dewpoint via: Pv = hrKg * ATM / (0.621945 + hrKg),
    // then Td = 243.04 * ln(Pv/611.657) / (17.625 - ln(Pv/611.657)).
    var dewpointAxisTicks = y.ticks(8).filter(function(v) { return v > 0; });
    var dewpointAxis = d3.svg.axis()
        .scale(y)
        .orient("left")
        .tickValues(dewpointAxisTicks)
        .tickFormat(function(hrDisp) {
            var hrKg = hrDisp / (isIP ? 7000 : 1000);
            if (hrKg <= 0) return "";
            var Pv = hrKg * ATM / (0.621945 + hrKg);
            if (Pv <= 0) return "";
            var lnPv = Math.log(Pv / 611.657);
            var tdC = 243.04 * lnPv / (17.625 - lnPv);
            return (isIP ? (32 + tdC * 1.8) : tdC).toFixed(1) + (isIP ? "\xB0F" : "\xB0C");
        });

    svg.append("g")
        .attr("class", "axis")
        .call(dewpointAxis);

    // x-axis label
    svg.append("text")
        .attr("x", width / 2)
        .attr("y", height + 42)
        .attr("text-anchor", "middle")
        .attr("font-size", "13px")
        .text(isIP ? "Dry Bulb Temperature (\xB0F)" : "Dry Bulb Temperature (\xB0C)");

    // y-axis label (right side, rotated)
    svg.append("text")
        .attr("transform", "rotate(-90)")
        .attr("x", -(height / 2))
        .attr("y", width + 45)
        .attr("text-anchor", "middle")
        .attr("font-size", "13px")
        .text(isIP ? "Humidity Ratio (gr/lb)" : "Humidity Ratio (g/kg)");

    // y-axis label (left side, rotated) - dewpoint temperature
    svg.append("text")
        .attr("transform", "rotate(-90)")
        .attr("x", -(height / 2))
        .attr("y", -margin.left + 12)
        .attr("text-anchor", "middle")
        .attr("font-size", "13px")
        .text(isIP ? "Dewpoint Temperature (\xB0F)" : "Dewpoint Temperature (\xB0C)");

    // --- Hours selected label -------------------------------------------
    var totalHours = displayData.length;
    var formatPct  = d3.format(".1%");

    var hoursLabel = svg.append("text")
        .attr("class", "legend-header")
        .attr("x", 0)
        .attr("y", -6)
        .attr("font-size", "14px")
        .attr("fill", "#555")
        .text(totalHours + " of " + totalHours + " hours (100%) selected");

    // --- Expose update function for crossfilter --------------------------
    window.updatePsychroPoints = function(filteredData) {
        if (!filteredData) return;
        var filteredSet = {};
        filteredData.forEach(function(d) { filteredSet[d.index] = true; });
        var activeDisplayData = displayData.filter(function(d) { return filteredSet[d.index]; });
        drawHeatmap(activeDisplayData);
        var n = activeDisplayData.length;
        hoursLabel.text(n + " of " + totalHours + " hours (" + formatPct(n / totalHours) + ") selected");
    };
};

/*-------------------------------------------------------------------------
 * epwScatterChart
 * X-Y scatter plot comparing two user-selected variables.
 * Responds to crossfilter: active hours colored, inactive greyed out.
 *-------------------------------------------------------------------------*/
function epwScatterChart(epw) {
    var isIP = (unitSystem === 'IP');

    var MONTH_NAMES  = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
    var HOUR_LABELS  = ['12am','1am','2am','3am','4am','5am','6am','7am','8am','9am','10am','11am',
                        '12pm','1pm','2pm','3pm','4pm','5pm','6pm','7pm','8pm','9pm','10pm','11pm'];

    var varDefs = [
        { id: 'dryBulbTemperature',         label: 'Dry Bulb Temperature',     unit: isIP ? '\u00B0F' : '\u00B0C',
          getVal: function(d) { return d.dryBulbTemperature; } },
        { id: 'dewPointTemperature',        label: 'Dew Point Temperature',    unit: isIP ? '\u00B0F' : '\u00B0C',
          getVal: function(d) { return d.dewPointTemperature; } },
        { id: 'wetBulbTemperature',         label: 'Wet Bulb Temperature',     unit: isIP ? '\u00B0F' : '\u00B0C',
          getVal: function(d) { return d.wetBulbTemperature; } },
        { id: 'relativeHumidity',           label: 'Relative Humidity',        unit: '%',
          getVal: function(d) { return d.relativeHumidity; } },
        { id: 'humidityRatio',              label: 'Humidity Ratio',           unit: isIP ? 'gr/lb' : 'g/kg',
          getVal: function(d) { return (d.humidityRatio !== null && isFinite(d.humidityRatio)) ? d.humidityRatio * (isIP ? 7000 : 1000) : null; } },
        { id: 'enthalpy',                   label: 'Enthalpy',                 unit: isIP ? 'BTU/lb' : 'kJ/kg',
          getVal: function(d) { return (d.moistAirEnthalpy !== null && isFinite(d.moistAirEnthalpy)) ? d.moistAirEnthalpy * (isIP ? 4.29923e-4 : 0.001) : null; } },
        { id: 'globalHorizontalRadiation',  label: 'Global Horiz. Radiation',  unit: 'Wh/m\u00B2',
          getVal: function(d) { return d.globalHorizontalRadiation; } },
        { id: 'directNormalRadiation',      label: 'Direct Normal Radiation',  unit: 'Wh/m\u00B2',
          getVal: function(d) { return d.directNormalRadiation; } },
        { id: 'diffuseHorizontalRadiation', label: 'Diffuse Horiz. Radiation', unit: 'Wh/m\u00B2',
          getVal: function(d) { return d.diffuseHorizontalRadiation; } },
        { id: 'windSpeed',                  label: 'Wind Speed',               unit: isIP ? 'knots' : 'm/s',
          getVal: function(d) { return isIP ? d.windSpeed * 1.94384 : d.windSpeed; } },
        { id: 'windDirection',               label: 'Wind Direction',            unit: '°',
          getVal: function(d) { return (+d.windDirection >= 999) ? null : +d.windDirection; } },
        { id: 'totalSkyCover',              label: 'Total Sky Cover',            unit: 'tenths',
          getVal: function(d) { return d.totalSkyCover; } },
        { id: 'opaqueSkyCover',              label: 'Opaque Sky Cover',           unit: 'tenths',
          getVal: function(d) { return d.opaqueSkyCover; } },
        { id: 'visibility',                  label: 'Visibility',                 unit: 'km',
          getVal: function(d) { return d.visibility; } },
        { id: 'ceilingHeight',               label: 'Ceiling Height',             unit: isIP ? 'ft' : 'm',
          getVal: function(d) { return d.ceilingHeight !== null ? d.ceilingHeight * (isIP ? 3.28084 : 1) : null; } },
        { id: 'precipitableWater',           label: 'Precipitable Water',         unit: 'mm',
          getVal: function(d) { return d.precipitableWater; } },
        { id: 'aerosolOpticalDepth',         label: 'Aerosol Optical Depth',      unit: '',
          fmt: function(v) { return Number(v).toPrecision(3); },
          getVal: function(d) { return d.aerosolOpticalDepth; } },
        { id: 'snowDepth',                   label: 'Snow Depth',                 unit: isIP ? 'in' : 'cm',
          getVal: function(d) { return d.snowDepth !== null ? d.snowDepth * (isIP ? 0.393701 : 1) : null; } },
        { id: 'daysSinceLastSnowfall',       label: 'Days Since Last Snowfall',   unit: 'days',
          getVal: function(d) { return d.daysSinceLastSnowfall; } },
        { id: 'albedo',                      label: 'Albedo',                     unit: '',
          fmt: function(v) { return Number(v).toPrecision(3); },
          getVal: function(d) { return d.albedo; } },
        { id: 'liquidPrecipitationDepth',    label: 'Liquid Precip. Depth',       unit: 'mm',
          getVal: function(d) { return d.liquidPrecipitationDepth; } },
        { id: 'liquidPrecipitationQuantity', label: 'Liquid Precip. Quantity',    unit: 'hr',
          getVal: function(d) { return d.liquidPrecipitationQuantity; } },
        { id: 'pressure',                   label: 'Station Pressure',           unit: isIP ? 'inHg' : 'Pa',
          getVal: function(d) { return (d.atmosphericStationPressure !== null && isFinite(d.atmosphericStationPressure)) ? d.atmosphericStationPressure * (isIP ? 2.95299830714e-4 : 1) : null; } },
        { id: 'pvAC',                       label: 'PV AC Output',               unit: 'kWh',
          fmt: function(v) { return Number(v).toPrecision(2); },
          getVal: function(d) { return window._pvHourlyAC ? ((window._pvHourlyAC[d.index] || 0) / 1000) : 0; } }
    ];

    function getVarDef(id) {
        for (var i = 0; i < varDefs.length; i++) { if (varDefs[i].id === id) return varDefs[i]; }
        return varDefs[0];
    }

    // Persist dropdown selections across unit-system re-renders
    if (!window._scatterXVar) window._scatterXVar = 'dryBulbTemperature';
    if (!window._scatterYVar) window._scatterYVar = 'relativeHumidity';

    function populateSelect(elId, selectedId) {
        var sel = document.getElementById(elId);
        if (!sel) return null;
        var clone = sel.cloneNode(false);
        sel.parentNode.replaceChild(clone, sel);
        sel = clone;
        sel.innerHTML = '';
        varDefs.forEach(function(v) {
            var opt = document.createElement('option');
            opt.value = v.id;
            opt.textContent = v.label + ' (' + v.unit + ')';
            if (v.id === selectedId) opt.selected = true;
            sel.appendChild(opt);
        });
        return sel;
    }

    var xSel = populateSelect('scatterXSelect', window._scatterXVar);
    var ySel = populateSelect('scatterYSelect', window._scatterYVar);

    // Build full data array once
    var rawData  = epwDataDefault(epw, unitSystem);
    var plotData = rawData.map(function(d) {
        return { index: d.index, month: d.month, day: d.day, hour: d.hour, _raw: d };
    });

    var totalHours = plotData.length;
    var formatPct  = d3.format('.1%');

    // Chart geometry
    var margin = { top: 20, right: 30, bottom: 60, left: 70 };
    var width  = 720;
    var height = 430;

    var svg = d3.select('#epwScatterChart').append('svg')
        .attr('width',  width  + margin.left + margin.right)
        .attr('height', height + margin.top  + margin.bottom)
        .append('g')
        .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');

    var xAxisG   = svg.append('g').attr('class', 'axis').attr('transform', 'translate(0,' + height + ')');
    var yAxisG   = svg.append('g').attr('class', 'axis');
    var xLabelEl = svg.append('text').attr('text-anchor', 'middle').attr('font-size', '13px')
                      .attr('y', height + 50).attr('x', width / 2);
    var yLabelEl = svg.append('text').attr('text-anchor', 'middle').attr('font-size', '13px')
                      .attr('transform', 'rotate(-90)').attr('x', -(height / 2)).attr('y', -55);
    var hoursLabel = svg.append('text').attr('class', 'legend-header')
                       .attr('x', 0).attr('y', -6)
                       .attr('font-size', '14px').attr('fill', '#555')
                       .text(totalHours + ' of ' + totalHours + ' hours (100%) selected');

    var dotsG = svg.append('g').attr('class', 'scatter-dots');

    var x, y, xVd, yVd;

    function redrawAxes() {
        xVd = getVarDef(xSel ? xSel.value : window._scatterXVar);
        yVd = getVarDef(ySel ? ySel.value : window._scatterYVar);

        var xVals = plotData.map(function(d) { return xVd.getVal(d._raw); }).filter(function(v) { return v !== null && isFinite(v); });
        var yVals = plotData.map(function(d) { return yVd.getVal(d._raw); }).filter(function(v) { return v !== null && isFinite(v); });
        var xMin = d3.min(xVals), xMax = d3.max(xVals);
        var yMin = d3.min(yVals), yMax = d3.max(yVals);
        var xPad = (xMax - xMin) * 0.03 || 1;
        var yPad = (yMax - yMin) * 0.03 || 1;

        x = d3.scale.linear().domain([xMin - xPad, xMax + xPad]).range([0, width]);
        y = d3.scale.linear().domain([yMin - yPad, yMax + yPad]).range([height, 0]);

        xAxisG.call(d3.svg.axis().scale(x).orient('bottom').ticks(8));
        yAxisG.call(d3.svg.axis().scale(y).orient('left').ticks(8));
        xLabelEl.text(xVd.label + ' (' + xVd.unit + ')');
        yLabelEl.text(yVd.label + ' (' + yVd.unit + ')');
    }

    function redrawDots(activeSet) {
        var plotPts = plotData.map(function(d) {
            return { index: d.index, month: d.month, day: d.day, hour: d.hour,
                     xv: xVd.getVal(d._raw), yv: yVd.getVal(d._raw) };
        }).filter(function(d) { return d.xv !== null && isFinite(d.xv) && d.yv !== null && isFinite(d.yv); });

        dotsG.selectAll('circle').remove();

        dotsG.selectAll('circle')
            .data(plotPts)
          .enter().append('circle')
            .attr('r', 2)
            .attr('cx', function(d) { return x(d.xv); })
            .attr('cy', function(d) { return y(d.yv); })
            .style('fill', function(d) { return (!activeSet || activeSet.has(d.index)) ? window._plotColor : '#ccc'; })
            .style('fill-opacity', function(d) { return (!activeSet || activeSet.has(d.index)) ? 0.55 : 0.2; })
            .style('stroke', 'none')
            .on('mouseover', function(d) {
                var tt = document.getElementById('epw-scatter-tooltip');
                if (!tt) return;
                var mName  = MONTH_NAMES[d.month - 1] || ('M' + d.month);
                var hLabel = HOUR_LABELS[d.hour - 1]  || (d.hour + ':00');
                tt.innerHTML = '<strong>' + mName + ' ' + d.day + '</strong>&nbsp;&nbsp;' + hLabel
                    + '<br>' + xVd.label + ': ' + (xVd.fmt ? xVd.fmt(d.xv) : d.xv.toFixed(2)) + '\u00A0' + xVd.unit
                    + '<br>' + yVd.label + ': ' + (yVd.fmt ? yVd.fmt(d.yv) : d.yv.toFixed(2)) + '\u00A0' + yVd.unit;
                tt.style.display = 'block';
                tt.style.left = (d3.event.clientX + 12) + 'px';
                tt.style.top  = (d3.event.clientY + 14) + 'px';
            })
            .on('mousemove', function() {
                var tt = document.getElementById('epw-scatter-tooltip');
                if (!tt) return;
                tt.style.left = (d3.event.clientX + 12) + 'px';
                tt.style.top  = (d3.event.clientY + 14) + 'px';
            })
            .on('mouseout', function() {
                var tt = document.getElementById('epw-scatter-tooltip');
                if (tt) tt.style.display = 'none';
            });
    }

    function fullRedraw(activeSet) {
        redrawAxes();
        redrawDots(activeSet);
    }

    fullRedraw(null);

    // Dropdown change handlers
    if (xSel) {
        xSel.addEventListener('change', function() {
            window._scatterXVar = this.value;
            fullRedraw(window._scatterLastActiveSet || null);
        });
    }
    if (ySel) {
        ySel.addEventListener('change', function() {
            window._scatterYVar = this.value;
            fullRedraw(window._scatterLastActiveSet || null);
        });
    }

    // Crossfilter update hook: re-color in-place (no axis/dot rebuild needed)
    window.updateScatter = function(filteredData) {
        if (!filteredData) return;
        var activeSet = new Set();
        filteredData.forEach(function(d) { activeSet.add(d.index); });
        window._scatterLastActiveSet = activeSet;
        dotsG.selectAll('circle')
            .style('fill', function(d) { return activeSet.has(d.index) ? window._plotColor : '#ccc'; })
            .style('fill-opacity', function(d) { return activeSet.has(d.index) ? 0.55 : 0.2; });
        var n = filteredData.length;
        hoursLabel.text(n + ' of ' + totalHours + ' hours (' + formatPct(n / totalHours) + ') selected');
    };
    // PV Recalculate hook: full redraw so dot positions reflect new pvAC getVal
    window._scatterFullRedraw = function() { fullRedraw(window._scatterLastActiveSet || null); };
};

/*-------------------------------------------------------------------------
 * epwTimeseriesCharts
 * Chart 1: Annual timeseries (daily avg + min/max band, scrollable)
 * Chart 2: Average daily profile for each month (hourly avg + min/max band)
 * Both charts respond to crossfilter: grey = all data, color = active data.
 * Solar fields (GHI, DNI, DHI): zeros (night hours) excluded from averages.
 *-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
 * epwTabularData
 * Crossfilter-responsive statistics table for each variable.
 *-------------------------------------------------------------------------*/
function epwTabularData(epw) {
    var isIP = (unitSystem === 'IP');

    var varDefs = [
        { id: 'dryBulbTemperature',        label: 'Dry Bulb Temperature',      unit: isIP ? '\u00B0F' : '\u00B0C',   isSolar: false },
        { id: 'dewPointTemperature',        label: 'Dew Point Temperature',     unit: isIP ? '\u00B0F' : '\u00B0C',   isSolar: false },
        { id: 'wetBulbTemperature',         label: 'Wet Bulb Temperature',      unit: isIP ? '\u00B0F' : '\u00B0C',   isSolar: false },
        { id: 'relativeHumidity',           label: 'Relative Humidity',         unit: '%',                             isSolar: false },
        { id: 'humidityRatio',              label: 'Humidity Ratio',            unit: isIP ? 'gr/lb' : 'g/kg',        isSolar: false },
        { id: 'enthalpy',                   label: 'Enthalpy',                  unit: isIP ? 'BTU/lb' : 'kJ/kg',      isSolar: false },
        { id: 'globalHorizontalRadiation',  label: 'Global Horiz. Radiation',   unit: 'Wh/m\u00B2',                   isSolar: true  },
        { id: 'directNormalRadiation',      label: 'Direct Normal Radiation',   unit: 'Wh/m\u00B2',                   isSolar: true  },
        { id: 'diffuseHorizontalRadiation', label: 'Diffuse Horiz. Radiation',  unit: 'Wh/m\u00B2',                   isSolar: true  },
        { id: 'windSpeed',                  label: 'Wind Speed',                unit: isIP ? 'knots' : 'm/s',          isSolar: false },
        { id: 'windDirection',              label: 'Wind Direction',            unit: '°',                          isSolar: false },
        { id: 'totalSkyCover',              label: 'Total Sky Cover',           unit: 'tenths',                        isSolar: false },
        { id: 'opaqueSkyCover',             label: 'Opaque Sky Cover',          unit: 'tenths',                        isSolar: false },
        { id: 'visibility',                 label: 'Visibility',                unit: 'km',                            isSolar: false },
        { id: 'ceilingHeight',              label: 'Ceiling Height',            unit: isIP ? 'ft' : 'm',               isSolar: false },
        { id: 'precipitableWater',          label: 'Precipitable Water',        unit: 'mm',                            isSolar: false },
        { id: 'aerosolOpticalDepth',        label: 'Aerosol Optical Depth',     unit: '',                              isSolar: false },
        { id: 'snowDepth',                  label: 'Snow Depth',                unit: isIP ? 'in' : 'cm',              isSolar: false },
        { id: 'daysSinceLastSnowfall',      label: 'Days Since Last Snowfall',  unit: 'days',                          isSolar: false },
        { id: 'albedo',                     label: 'Albedo',                    unit: '',                              isSolar: false },
        { id: 'liquidPrecipitationDepth',   label: 'Liquid Precip. Depth',      unit: 'mm',                            isSolar: false },
        { id: 'liquidPrecipitationQuantity',label: 'Liquid Precip. Quantity',   unit: 'hr',                            isSolar: false },
        { id: 'pressure',                   label: 'Station Pressure',          unit: isIP ? 'inHg' : 'Pa',            isSolar: false },
        { id: 'pvAC',                       label: 'PV AC Output',              unit: 'kWh',                           isSolar: true,
          tblFmt: function(v) { return (v === null || !isFinite(v)) ? '\u2014' : Number(v).toPrecision(3); } }
    ];

    var MONTH_NAMES = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];

    // ---- Populate variable selector ----
    var sel = document.getElementById('tblVariableSelect');
    if (!sel) return;
    var selClone = sel.cloneNode(false);
    sel.parentNode.replaceChild(selClone, sel);
    sel = selClone;
    sel.innerHTML = '';
    varDefs.forEach(function(v) {
        var opt = document.createElement('option');
        opt.value = v.id;
        opt.textContent = v.label + ' (' + v.unit + ')';
        sel.appendChild(opt);
    });

    // Restore last-selected variable across re-renders
    if (window._tblCurrentVarId) {
        sel.value = window._tblCurrentVarId;
        if (!sel.value) sel.value = varDefs[0].id;
    }

    // ---- Build full enriched dataset (mirrors epwTimeseriesCharts logic) ----
    function buildAllData() {
        var months = epw.month(), days = epw.day(), hours = epw.hour();
        var n = months.length;
        var dbt  = epw.dryBulbTemperature();
        var dpt  = epw.dewPointTemperature();
        var wbt  = epw.wetBulbTemperature();
        var rh   = epw.relativeHumidity();
        var hr   = epw.humidityRatio();
        var enth = epw.moistAirEnthalpy();
        var ghi  = epw.globalHorizontalRadiation();
        var dni  = epw.directNormalRadiation();
        var dhi  = epw.diffuseHorizontalRadiation();
        var ws   = epw.windSpeed();
        var wd   = epw.windDirection().map(function(v) { return (+v >= 999) ? null : +v; });
        var tsc  = epw.totalSkyCover();
        var pres = epw.atmosphericStationPressure();
        var osc  = epw.opaqueSkyCover();
        var vis  = epw.visibility().map(function(v) { return (+v >= 9999) ? null : +v; });
        var ceil = epw.ceilingHeight().map(function(v) { return (+v >= 99999) ? null : +v; });
        var pw   = epw.precipitableWater().map(function(v) { return (+v >= 999) ? null : +v; });
        var aod  = epw.aerosolOpticalDepth().map(function(v) { return (+v >= 0.999) ? null : +v; });
        var snow = epw.snowDepth().map(function(v) { return (+v >= 999) ? null : +v; });
        var dsls = epw.daysSinceLastSnowfall().map(function(v) { return (+v >= 99) ? null : +v; });
        var alb  = epw.albedo().map(function(v) { return (+v >= 2) ? null : +v; });
        var lpd  = epw.liquidPrecipitationDepth().map(function(v) { return (+v >= 999) ? null : +v; });
        var lpq  = epw.liquidPrecipitationQuantity().map(function(v) { return (+v >= 99) ? null : +v; });
        if (isIP) {
            dbt  = convertCtoF(dbt);
            dpt  = dpt.map(function(v)  { return (v !== null && isFinite(v)) ? 32 + v * 1.8        : null; });
            wbt  = wbt.map(function(v)  { return (v !== null && isFinite(v)) ? 32 + v * 1.8        : null; });
            hr   = hr.map(function(v)   { return (v !== null && isFinite(v)) ? v * 7000             : null; });
            enth = enth.map(function(v) { return (v !== null && isFinite(v)) ? v * 4.29923e-4       : null; });
            ws   = convertKnots(ws);
            pres = pres.map(function(v) { return (v !== null && isFinite(v)) ? v * 2.95299830714e-4 : null; });
            ceil = ceil.map(function(v) { return (v !== null && isFinite(v)) ? v * 3.28084          : null; });
            snow = snow.map(function(v) { return (v !== null && isFinite(v)) ? v * 0.393701         : null; });
        } else {
            wbt  = wbt.slice();
            hr   = hr.map(function(v)   { return (v !== null && isFinite(v)) ? v * 1000             : null; });
            enth = enth.map(function(v) { return (v !== null && isFinite(v)) ? v * 0.001            : null; });
            pres = pres.slice();
            ceil = ceil.slice();
            snow = snow.slice();
        }
        var data = [];
        for (var i = 0; i < n; i++) {
            data.push({
                index: i, month: months[i],
                dryBulbTemperature:        dbt[i],
                dewPointTemperature:       dpt[i],
                wetBulbTemperature:        wbt[i],
                relativeHumidity:          rh[i],
                humidityRatio:             hr[i],
                enthalpy:                  enth[i],
                globalHorizontalRadiation: ghi[i],
                directNormalRadiation:     dni[i],
                diffuseHorizontalRadiation:dhi[i],
                windSpeed:                 ws[i],
                windDirection:             wd[i],
                totalSkyCover:             tsc[i],
                opaqueSkyCover:            osc[i],
                visibility:                vis[i],
                ceilingHeight:             ceil[i],
                precipitableWater:         pw[i],
                aerosolOpticalDepth:       aod[i],
                snowDepth:                 snow[i],
                daysSinceLastSnowfall:     dsls[i],
                albedo:                    alb[i],
                liquidPrecipitationDepth:  lpd[i],
                liquidPrecipitationQuantity: lpq[i],
                pressure:                  pres[i],
                pvAC:                      window._pvHourlyAC ? ((window._pvHourlyAC[i] || 0) / 1000) : 0
            });
        }
        return data;
    }

    var allData = buildAllData();
    var totalHours = allData.length;

    // ---- Statistics helpers ----
    function percentile(sorted, p) {
        if (!sorted.length) return null;
        var idx = (p / 100) * (sorted.length - 1);
        var lo = Math.floor(idx), hi = Math.ceil(idx);
        return sorted[lo] + (sorted[hi] - sorted[lo]) * (idx - lo);
    }

    function computeStats(values) {
        var v = values.filter(function(x) { return x !== null && isFinite(x); });
        if (!v.length) return { max: null, min: null, mean: null, std: null, p1: null, p99: null };
        var sum = 0, n = v.length;
        for (var i = 0; i < n; i++) sum += v[i];
        var mean = sum / n;
        var varS = 0;
        for (var i = 0; i < n; i++) varS += (v[i] - mean) * (v[i] - mean);
        var sorted = v.slice().sort(function(a, b) { return a - b; });
        return {
            max:  sorted[n - 1],
            min:  sorted[0],
            mean: mean,
            std:  Math.sqrt(varS / n),
            p1:   percentile(sorted, 1),
            p99:  percentile(sorted, 99)
        };
    }

    function fmt(v) {
        return (v === null || !isFinite(v)) ? '—' : v.toFixed(2);
    }

    // ---- Build / rebuild the table from a filtered record set ----
    function renderTable(activeData, varId) {
        window._tblCurrentVarId = varId;
        var vd = varDefs.filter(function(v) { return v.id === varId; })[0] || varDefs[0];
        var fmtCell = vd.tblFmt || fmt;

        // Update filter info bar
        var n = activeData.length;
        var pct = totalHours > 0 ? (n / totalHours * 100).toFixed(1) : '0.0';
        var infoEl = document.getElementById('tblFilterInfo');
        if (infoEl) infoEl.textContent = n + ' of ' + totalHours + ' hours (' + pct + '%) selected';

        // Group active data by month, building index set for fast lookup
        var activeSet = new Set();
        activeData.forEach(function(d) { activeSet.add(d.index); });

        var byMonth = {};
        for (var m = 1; m <= 12; m++) byMonth[m] = [];
        allData.forEach(function(d) {
            if (activeSet.has(d.index)) {
                byMonth[d.month].push(d[varId]);
            }
        });
        var allVals = [];
        allData.forEach(function(d) {
            if (activeSet.has(d.index)) allVals.push(d[varId]);
        });

        var u = vd.unit ? ' (' + vd.unit + ')' : '';
        var cols = [
            'Max' + u, 'Min' + u, 'Mean' + u, 'Std Dev' + u, '1%' + u, '99%' + u
        ];

        // Build HTML table
        var html = '<table><thead><tr><th>Month</th>';
        cols.forEach(function(c) { html += '<th>' + c + '</th>'; });
        html += '</tr></thead><tbody>';

        var rowData = []; // for CSV
        for (var m = 1; m <= 12; m++) {
            var s = computeStats(byMonth[m]);
            var row = [MONTH_NAMES[m - 1], fmtCell(s.max), fmtCell(s.min), fmtCell(s.mean), fmtCell(s.std), fmtCell(s.p1), fmtCell(s.p99)];
            rowData.push(row);
            html += '<tr><td>' + row.join('</td><td>') + '</td></tr>';
        }
        var sAll = computeStats(allVals);
        var yearRow = ['Year', fmtCell(sAll.max), fmtCell(sAll.min), fmtCell(sAll.mean), fmtCell(sAll.std), fmtCell(sAll.p1), fmtCell(sAll.p99)];
        rowData.push(yearRow);
        html += '<tr class="tbl-year-row"><td>' + yearRow.join('</td><td>') + '</td></tr>';
        html += '</tbody></table>';

        var container = document.getElementById('tblTableContainer');
        if (container) container.innerHTML = html;

        // Store for CSV download
        window._tblLastCols    = ['Month'].concat(cols);
        window._tblLastRows    = rowData;
        window._tblLastVarDef  = vd;
    }

    // ---- Initial render with all data ----
    var currentActiveData = allData;
    renderTable(allData, sel.value);

    // ---- Variable selector change ----
    sel.addEventListener('change', function() {
        renderTable(currentActiveData, this.value);
    });

    // ---- CSV download ----
    var csvBtn = document.getElementById('tblDownloadCSV');
    if (csvBtn) {
        var csvBtnClone = csvBtn.cloneNode(true);
        csvBtn.parentNode.replaceChild(csvBtnClone, csvBtn);
        csvBtnClone.addEventListener('click', function() {
            var cols = window._tblLastCols;
            var rows = window._tblLastRows;
            var vd   = window._tblLastVarDef;
            if (!cols || !rows) return;
            var lines = [cols.map(function(c) { return '"' + c.replace(/"/g, '""') + '"'; }).join(',')];
            rows.forEach(function(r) {
                lines.push(r.map(function(c) { return '"' + String(c).replace(/"/g, '""') + '"'; }).join(','));
            });
            var blob = new Blob([lines.join('\r\n')], { type: 'text/csv;charset=utf-8;' });
            var base = (vd ? vd.label.toLowerCase().replace(/[^a-z0-9]+/g, '-') : 'tabular-data');
            triggerBlobDownload(blob, base + '-statistics.csv');
        });
    }

    // ---- Crossfilter update hook ----
    window.updateTabular = function(filteredCFData) {
        // Map CF records back to enriched allData rows by index
        var activeSet = new Set();
        filteredCFData.forEach(function(d) { activeSet.add(d.index); });
        currentActiveData = allData.filter(function(d) { return activeSet.has(d.index); });
        renderTable(currentActiveData, sel.value);
    };
    // Expose allData ref so PV Recalculate can patch pvAC values
    window._tblAllData = allData;
}

function epwTimeseriesCharts(epw) {
    var isIP = (unitSystem === 'IP');

    // ---- Variable definitions ----
    var varDefs = [
        { id: 'dryBulbTemperature',         label: 'Dry Bulb Temperature',       unit: isIP ? '\u00B0F'   : '\u00B0C',    isSolar: false },
        { id: 'dewPointTemperature',         label: 'Dew Point Temperature',      unit: isIP ? '\u00B0F'   : '\u00B0C',    isSolar: false },
        { id: 'wetBulbTemperature',          label: 'Wet Bulb Temperature',       unit: isIP ? '\u00B0F'   : '\u00B0C',    isSolar: false },
        { id: 'relativeHumidity',            label: 'Relative Humidity',          unit: '%',                                isSolar: false },
        { id: 'humidityRatio',               label: 'Humidity Ratio',             unit: isIP ? 'gr/lb'    : 'g/kg',        isSolar: false },
        { id: 'enthalpy',                    label: 'Enthalpy',                   unit: isIP ? 'BTU/lb'   : 'kJ/kg',       isSolar: false },
        { id: 'globalHorizontalRadiation',   label: 'Global Horiz. Radiation',    unit: 'Wh/m\u00B2',                      isSolar: true  },
        { id: 'directNormalRadiation',       label: 'Direct Normal Radiation',    unit: 'Wh/m\u00B2',                      isSolar: true  },
        { id: 'diffuseHorizontalRadiation',  label: 'Diffuse Horiz. Radiation',   unit: 'Wh/m\u00B2',                      isSolar: true  },
        { id: 'windSpeed',                   label: 'Wind Speed',                 unit: isIP ? 'knots'    : 'm/s',          isSolar: false },
        { id: 'windDirection',               label: 'Wind Direction',             unit: '°',                           isSolar: false },
        { id: 'totalSkyCover',               label: 'Total Sky Cover',            unit: 'tenths',                           isSolar: false },
        { id: 'opaqueSkyCover',              label: 'Opaque Sky Cover',           unit: 'tenths',                           isSolar: false },
        { id: 'visibility',                  label: 'Visibility',                 unit: 'km',                               isSolar: false },
        { id: 'ceilingHeight',               label: 'Ceiling Height',             unit: isIP ? 'ft' : 'm',                  isSolar: false },
        { id: 'precipitableWater',           label: 'Precipitable Water',         unit: 'mm',                               isSolar: false },
        { id: 'aerosolOpticalDepth',         label: 'Aerosol Optical Depth',      unit: '',                                 isSolar: false },
        { id: 'snowDepth',                   label: 'Snow Depth',                 unit: isIP ? 'in' : 'cm',                 isSolar: false },
        { id: 'daysSinceLastSnowfall',       label: 'Days Since Last Snowfall',   unit: 'days',                             isSolar: false },
        { id: 'albedo',                      label: 'Albedo',                     unit: '',                                 isSolar: false },
        { id: 'liquidPrecipitationDepth',    label: 'Liquid Precip. Depth',       unit: 'mm',                               isSolar: false },
        { id: 'liquidPrecipitationQuantity', label: 'Liquid Precip. Quantity',    unit: 'hr',                               isSolar: false },
        { id: 'pressure',                    label: 'Station Pressure',           unit: isIP ? 'inHg'     : 'Pa',           isSolar: false },
        { id: 'pvAC',                        label: 'PV AC Output',               unit: 'kWh',                              isSolar: true,
          fmt: function(v) { return Number(v).toPrecision(2); } }
    ];

    // Populate the dropdown; clone element first to strip any stale listeners
    var sel = document.getElementById('tsVariableSelect');
    if (!sel) return;
    var selClone = sel.cloneNode(false);
    sel.parentNode.replaceChild(selClone, sel);
    sel = selClone;
    sel.innerHTML = '';
    varDefs.forEach(function(v) {
        var opt = document.createElement('option');
        opt.value       = v.id;
        opt.textContent = v.label + ' (' + v.unit + ')';
        sel.appendChild(opt);
    });

    function getVarDef(id) {
        for (var i = 0; i < varDefs.length; i++) {
            if (varDefs[i].id === id) return varDefs[i];
        }
        return varDefs[0];
    }

    // ---- Build enriched data array (unit-converted) ----
    function buildAllData() {
        var months = epw.month(), days = epw.day(), hours = epw.hour();
        var n = months.length;

        var dbt  = epw.dryBulbTemperature();
        var dpt  = epw.dewPointTemperature();
        var wbt  = epw.wetBulbTemperature();        // may contain nulls
        var rh   = epw.relativeHumidity();
        var hr   = epw.humidityRatio();              // kg/kg (psychrolib)
        var enth = epw.moistAirEnthalpy();           // J/kg  (psychrolib)
        var ghi  = epw.globalHorizontalRadiation();
        var dni  = epw.directNormalRadiation();
        var dhi  = epw.diffuseHorizontalRadiation();
        var ws   = epw.windSpeed();
        var wd   = epw.windDirection().map(function(v) { return (+v >= 999) ? null : +v; });
        var tsc  = epw.totalSkyCover();
        var pres = epw.atmosphericStationPressure();
        var osc  = epw.opaqueSkyCover();
        var vis  = epw.visibility().map(function(v) { return (+v >= 9999) ? null : +v; });
        var ceil = epw.ceilingHeight().map(function(v) { return (+v >= 99999) ? null : +v; });
        var pw   = epw.precipitableWater().map(function(v) { return (+v >= 999) ? null : +v; });
        var aod  = epw.aerosolOpticalDepth().map(function(v) { return (+v >= 0.999) ? null : +v; });
        var snow = epw.snowDepth().map(function(v) { return (+v >= 999) ? null : +v; });
        var dsls = epw.daysSinceLastSnowfall().map(function(v) { return (+v >= 99) ? null : +v; });
        var alb  = epw.albedo().map(function(v) { return (+v >= 2) ? null : +v; });
        var lpd  = epw.liquidPrecipitationDepth().map(function(v) { return (+v >= 999) ? null : +v; });
        var lpq  = epw.liquidPrecipitationQuantity().map(function(v) { return (+v >= 99) ? null : +v; });

        if (isIP) {
            dbt  = convertCtoF(dbt);
            dpt  = dpt.map(function(v)  { return (v !== null && isFinite(v)) ? 32 + v * 1.8       : null; });
            wbt  = wbt.map(function(v)  { return (v !== null && isFinite(v)) ? 32 + v * 1.8       : null; });
            hr   = hr.map(function(v)   { return (v !== null && isFinite(v)) ? v * 7000            : null; }); // gr/lb
            enth = enth.map(function(v) { return (v !== null && isFinite(v)) ? v * 4.29923e-4      : null; }); // BTU/lb
            ws   = convertKnots(ws);
            pres = pres.map(function(v) { return (v !== null && isFinite(v)) ? v * 2.95299830714e-4: null; }); // inHg
            ceil = ceil.map(function(v) { return (v !== null && isFinite(v)) ? v * 3.28084         : null; }); // ft
            snow = snow.map(function(v) { return (v !== null && isFinite(v)) ? v * 0.393701        : null; }); // in
        } else {
            wbt  = wbt.slice();
            hr   = hr.map(function(v)   { return (v !== null && isFinite(v)) ? v * 1000            : null; }); // g/kg
            enth = enth.map(function(v) { return (v !== null && isFinite(v)) ? v * 0.001           : null; }); // kJ/kg
            pres = pres.slice();
            ceil = ceil.slice();
            snow = snow.slice();
        }

        var data = [];
        for (var i = 0; i < n; i++) {
            data.push({
                index:                      i,
                month:                      months[i],
                day:                        days[i],
                hour:                       hours[i],
                dayOfYear:                  Math.floor(i / 24) + 1,
                dryBulbTemperature:         dbt[i],
                dewPointTemperature:        (dpt[i] !== null && isFinite(dpt[i]))  ? dpt[i]  : null,
                wetBulbTemperature:         (wbt[i] !== null && isFinite(wbt[i]))  ? wbt[i]  : null,
                relativeHumidity:           rh[i],
                humidityRatio:              hr[i],
                enthalpy:                   enth[i],
                globalHorizontalRadiation:  ghi[i],
                directNormalRadiation:      dni[i],
                diffuseHorizontalRadiation: dhi[i],
                windSpeed:                  ws[i],
                windDirection:              wd[i],
                totalSkyCover:              tsc[i],
                opaqueSkyCover:             osc[i],
                visibility:                 vis[i],
                ceilingHeight:              ceil[i],
                precipitableWater:          pw[i],
                aerosolOpticalDepth:        aod[i],
                snowDepth:                  snow[i],
                daysSinceLastSnowfall:      dsls[i],
                albedo:                     alb[i],
                liquidPrecipitationDepth:   lpd[i],
                liquidPrecipitationQuantity: lpq[i],
                pressure:                   (+pres[i]) || null,
                pvAC:                       window._pvHourlyAC ? ((window._pvHourlyAC[i] || 0) / 1000) : 0
            });
        }
        return data;
    }

    var allData = buildAllData();
    window._tsAllData       = allData;
    window._tsCurrentVarId  = sel.value || 'dryBulbTemperature';
    window._tsLastActive    = null;

    // ---- Aggregation: daily stats (Chart 1) ----
    function computeDailyStats(data, varDef) {
        var groups = {};
        var isSolar = varDef.isSolar;
        data.forEach(function(d) {
            var v = d[varDef.id];
            if (v === null || !isFinite(v)) return;
            if (isSolar && v === 0) return;
            var key = d.dayOfYear;
            if (!groups[key]) groups[key] = [];
            groups[key].push(v);
        });
        var result = [];
        for (var day = 1; day <= 365; day++) {
            var arr = groups[day];
            if (!arr || arr.length === 0) {
                result.push({ day: day, avg: null, min: null, max: null });
            } else {
                var sum = 0;
                for (var k = 0; k < arr.length; k++) sum += arr[k];
                result.push({
                    day: day,
                    avg: sum / arr.length,
                    min: Math.min.apply(Math, arr),
                    max: Math.max.apply(Math, arr)
                });
            }
        }
        return result;
    }

    // ---- Aggregation: hourly profile per month (Chart 2) ----
    function computeHourlyProfileByMonth(data, varDef) {
        var groups = {};
        var isSolar = varDef.isSolar;
        data.forEach(function(d) {
            var v = d[varDef.id];
            if (v === null || !isFinite(v)) return;
            if (isSolar && v === 0) return;
            var key = d.month + '_' + d.hour;
            if (!groups[key]) groups[key] = [];
            groups[key].push(v);
        });
        var result = {};
        for (var m = 1; m <= 12; m++) {
            result[m] = {};
            for (var h = 1; h <= 24; h++) {
                var arr = groups[m + '_' + h];
                if (!arr || arr.length === 0) {
                    result[m][h] = { avg: null, min: null, max: null };
                } else {
                    var sum = 0;
                    for (var k = 0; k < arr.length; k++) sum += arr[k];
                    result[m][h] = {
                        avg: sum / arr.length,
                        min: Math.min.apply(Math, arr),
                        max: Math.max.apply(Math, arr)
                    };
                }
            }
        }
        return result;
    }

    // ---- Chart 1: Annual Timeseries ----
    function drawAnnualTimeseries(activeData, varDef) {
        var container = d3.select('#epwTimeseriesAnnual');
        container.selectAll('*').remove();

        var allStats    = computeDailyStats(allData,    varDef);
        var activeStats = computeDailyStats(activeData, varDef);

        var allMaxes = allStats.filter(function(d) { return d.max !== null; }).map(function(d) { return d.max; });
        var allMins  = allStats.filter(function(d) { return d.min !== null; }).map(function(d) { return d.min; });
        if (allMaxes.length === 0) return;

        var yRawMin = Math.min.apply(Math, allMins);
        var yRawMax = Math.max.apply(Math, allMaxes);
        var yPad    = Math.max((yRawMax - yRawMin) * 0.06, 0.5);

        var margin  = { top: 20, right: 20, bottom: 40, left: 62 };
        var pxPerDay = 2.5;
        var innerW   = Math.round(pxPerDay * 365);
        var innerH   = 240;
        var totalW   = innerW + margin.left + margin.right;
        var totalH   = innerH + margin.top  + margin.bottom;

        var svg = container.append('svg')
            .attr('width',  totalW)
            .attr('height', totalH);

        var g = svg.append('g')
            .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');

        var xScale = d3.scale.linear().domain([1, 365]).range([0, innerW]);
        var yScale = d3.scale.linear().domain([yRawMin - yPad, yRawMax + yPad]).range([innerH, 0]);

        // ---- All-data grey band + line ----
        var areaAll = d3.svg.area()
            .x(function(d)  { return xScale(d.day); })
            .y0(function(d) { return yScale(d.min); })
            .y1(function(d) { return yScale(d.max); })
            .defined(function(d) { return d.min !== null; });

        g.append('path')
            .datum(allStats).attr('d', areaAll)
            .style('fill', '#ddd').style('fill-opacity', 0.9);

        var lineAll = d3.svg.line()
            .x(function(d) { return xScale(d.day); })
            .y(function(d) { return yScale(d.avg); })
            .defined(function(d) { return d.avg !== null; });

        g.append('path')
            .datum(allStats).attr('d', lineAll)
            .style('fill', 'none').style('stroke', '#aaa').style('stroke-width', '1.5px');

        // ---- Active (filtered) band + line ----
        var areaActive = d3.svg.area()
            .x(function(d)  { return xScale(d.day); })
            .y0(function(d) { return yScale(d.min); })
            .y1(function(d) { return yScale(d.max); })
            .defined(function(d) { return d.min !== null; });

        g.append('path')
            .datum(activeStats).attr('d', areaActive)
            .style('fill', window._plotColor || 'steelblue').style('fill-opacity', 0.35);

        var lineActive = d3.svg.line()
            .x(function(d) { return xScale(d.day); })
            .y(function(d) { return yScale(d.avg); })
            .defined(function(d) { return d.avg !== null; });

        g.append('path')
            .datum(activeStats).attr('d', lineActive)
            .style('fill', 'none').style('stroke', window._plotColor || 'steelblue').style('stroke-width', '2px');

        // ---- X Axis (months) ----
        var monthStarts = [1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335];
        var monthNames  = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];

        var xAxis = d3.svg.axis()
            .scale(xScale).orient('bottom')
            .tickValues(monthStarts)
            .tickFormat(function(d, i) { return monthNames[i]; });

        g.append('g')
            .attr('class', 'axis')
            .attr('transform', 'translate(0,' + innerH + ')')
            .call(xAxis);

        // Light vertical month separators
        monthStarts.forEach(function(d) {
            g.append('line')
                .attr('x1', xScale(d)).attr('x2', xScale(d))
                .attr('y1', 0).attr('y2', innerH)
                .style('stroke', '#e0e0e0').style('stroke-width', '0.5px');
        });

        // ---- Y Axis ----
        var yAxis = d3.svg.axis().scale(yScale).orient('left').ticks(6);
        g.append('g').attr('class', 'axis').call(yAxis);

        g.append('text')
            .attr('transform', 'rotate(-90)')
            .attr('x', -innerH / 2).attr('y', -margin.left + 14).attr('dy', '1em')
            .style('text-anchor', 'middle').style('font-size', '13px')
            .text(varDef.label + ' (' + varDef.unit + ')');

        // ---- Tooltip ----
        var fmtV = d3.format('.1f');

        // Day-of-year → "Mmm D" label
        function doyToLabel(doy) {
            var mStarts = [1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335];
            var mNames  = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
            var mi = 0;
            for (var i = 0; i < 12; i++) { if (doy >= mStarts[i]) mi = i; }
            return mNames[mi] + ' ' + (doy - mStarts[mi] + 1);
        }

        // Fast day-indexed lookups
        var allByDay    = {};
        var activeByDay = {};
        allStats.forEach(function(d)    { allByDay[d.day]    = d; });
        activeStats.forEach(function(d) { activeByDay[d.day] = d; });

        // Shared tooltip div (created once, reused on redraw)
        var ttip1Id = 'epw-ts1-tooltip';
        var ttip1 = d3.select('body').select('#' + ttip1Id);
        if (ttip1.empty()) {
            ttip1 = d3.select('body').append('div').attr('id', ttip1Id);
        }
        ttip1.style({
            'position':       'fixed',
            'pointer-events': 'none',
            'background':     'rgba(255,255,255,0.95)',
            'border':         '1px solid #ccc',
            'border-radius':  '4px',
            'padding':        '6px 9px',
            'font-size':      '11px',
            'line-height':    '1.6',
            'box-shadow':     '0 2px 6px rgba(0,0,0,0.15)',
            'display':        'none',
            'white-space':    'nowrap',
            'z-index':        '9999'
        });

        // Vertical crosshair line (rendered above data, below overlay)
        var crosshair1 = g.append('line')
            .attr('y1', 0).attr('y2', innerH)
            .style('stroke', '#666').style('stroke-width', '1px')
            .style('stroke-dasharray', '4,3')
            .style('pointer-events', 'none')
            .style('display', 'none');

        // Invisible overlay rect captures all mouse events
        g.append('rect')
            .attr('width', innerW).attr('height', innerH)
            .style('fill', 'none').style('pointer-events', 'all')
            .on('mousemove', function() {
                var mx  = d3.mouse(this)[0];
                var doy = Math.max(1, Math.min(365, Math.round(xScale.invert(mx))));
                var a   = allByDay[doy];
                var f   = activeByDay[doy];
                if (!a) return;

                var html = '<strong>' + doyToLabel(doy) + ' (day ' + doy + ')</strong><br>';
                html += '<span style="color:#999">All &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; '
                    + 'min: ' + (a.min !== null ? fmtV(a.min) : '\u2013') + '&nbsp;'
                    + varDef.unit + ' &nbsp; avg: ' + (a.avg !== null ? fmtV(a.avg) : '\u2013') + '&nbsp;'
                    + varDef.unit + ' &nbsp; max: ' + (a.max !== null ? fmtV(a.max) : '\u2013') + '&nbsp;'
                    + varDef.unit + '</span><br>';
                html += '<span style="color:steelblue">Filtered '
                    + 'min: ' + (f && f.min !== null ? fmtV(f.min) : '\u2013') + '&nbsp;'
                    + varDef.unit + ' &nbsp; avg: ' + (f && f.avg !== null ? fmtV(f.avg) : '\u2013') + '&nbsp;'
                    + varDef.unit + ' &nbsp; max: ' + (f && f.max !== null ? fmtV(f.max) : '\u2013') + '&nbsp;'
                    + varDef.unit + '</span>';

                ttip1.html(html)
                    .style('display', 'block')
                    .style('left', (d3.event.clientX + 14) + 'px')
                    .style('top',  (d3.event.clientY - 42) + 'px');

                crosshair1
                    .attr('x1', mx).attr('x2', mx)
                    .style('display', null);
            })
            .on('mouseleave', function() {
                ttip1.style('display', 'none');
                crosshair1.style('display', 'none');
            });
    }

    // ---- Chart 2: Daily profiles by month ----
    var MONTH_COLORS = [
        '#e41a1c','#ff7f00','#ccb800','#4daf4a',
        '#00b5ad','#377eb8','#984ea3','#f781bf',
        '#a65628','#888888','#66c2a5','#fc8d62'
    ];
    var MONTH_NAMES = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
    var HOURS = d3.range(1, 25);

    function drawDailyProfiles(activeData, varDef) {
        var container = d3.select('#epwTimeseriesDailyProfile');
        container.selectAll('*').remove();

        var allStats    = computeHourlyProfileByMonth(allData,    varDef);
        var activeStats = computeHourlyProfileByMonth(activeData, varDef);

        // Shared Y domain across all months
        var allMaxes = [], allMins = [];
        for (var m = 1; m <= 12; m++) {
            for (var h = 1; h <= 24; h++) {
                var s = allStats[m][h];
                if (s.max !== null) { allMaxes.push(s.max); allMins.push(s.min); }
            }
        }
        if (allMaxes.length === 0) return;

        var yRawMin = Math.min.apply(Math, allMins);
        var yRawMax = Math.max.apply(Math, allMaxes);
        var yPad    = Math.max((yRawMax - yRawMin) * 0.06, 0.5);

        // Small-multiples layout constants — totalW forced to match Chart 1
        var panelInnerH  = 170;
        var panelGutter  = 4;
        var marginLeft   = 56;   // space for shared y-axis
        var marginTop    = 22;   // space for month name labels
        var marginBottom = 30;   // space for x-axis ticks
        var marginRight  = 8;

        // Match the total SVG width of Chart 1 exactly
        var totalW = Math.round(2.5 * 365) + 62 + 20;  // 995px
        var panelInnerW = Math.floor((totalW - marginLeft - 11 * panelGutter - marginRight) / 12);
        var totalH = marginTop + panelInnerH + marginBottom;

        var svg = container.append('svg')
            .attr('width',  totalW)
            .attr('height', totalH);

        // Shared scales (same for every panel)
        var yScale = d3.scale.linear()
            .domain([yRawMin - yPad, yRawMax + yPad])
            .range([panelInnerH, 0]);

        var xScale = d3.scale.linear()
            .domain([1, 24])
            .range([0, panelInnerW]);

        // Shared area / line generators (use local-panel coordinates)
        var areaFn = d3.svg.area()
            .x(function(d)  { return xScale(d.h); })
            .y0(function(d) { return yScale(d.min); })
            .y1(function(d) { return yScale(d.max); })
            .defined(function(d) { return d.min !== null && d.max !== null; });

        var lineFn = d3.svg.line()
            .x(function(d) { return xScale(d.h); })
            .y(function(d) { return yScale(d.avg); })
            .defined(function(d) { return d.avg !== null; });

        // Sparse x-tick values that fit in a narrow panel
        var xTickValues = [6, 12, 18];
        var xTickFmt    = { 6: '6a', 12: '12p', 18: '6p' };

        // Shared y-axis on the left margin
        var yAxis = d3.svg.axis().scale(yScale).orient('left').ticks(5);
        svg.append('g')
            .attr('class', 'axis')
            .attr('transform', 'translate(' + marginLeft + ',' + marginTop + ')')
            .call(yAxis);

        // Y-axis label
        svg.append('text')
            .attr('transform', 'rotate(-90)')
            .attr('x', -(marginTop + panelInnerH / 2))
            .attr('y', 13)
            .style('text-anchor', 'middle')
            .style('font-size', '13px')
            .text(varDef.label + ' (' + varDef.unit + ')');

        // Hour number → readable label (EPW hour 1 = 12am–1am, hour 13 = 12pm–1pm)
        function hourLabel(h) {
            if (h === 24) return '12am';
            if (h === 12) return '12pm';
            return h < 12 ? h + 'am' : (h - 12) + 'pm';
        }
        var fmtV2 = varDef.fmt ? varDef.fmt : d3.format('.1f');

        // Shared tooltip div for chart 2
        var ttip2Id = 'epw-ts2-tooltip';
        var ttip2 = d3.select('body').select('#' + ttip2Id);
        if (ttip2.empty()) {
            ttip2 = d3.select('body').append('div').attr('id', ttip2Id);
        }
        ttip2.style({
            'position':       'fixed',
            'pointer-events': 'none',
            'background':     'rgba(255,255,255,0.95)',
            'border':         '1px solid #ccc',
            'border-radius':  '4px',
            'padding':        '6px 9px',
            'font-size':      '11px',
            'line-height':    '1.6',
            'box-shadow':     '0 2px 6px rgba(0,0,0,0.15)',
            'display':        'none',
            'white-space':    'nowrap',
            'z-index':        '9999'
        });

        // Draw one panel per month
        for (var mi = 1; mi <= 12; mi++) {
            (function(m) {
                var color  = window._plotColor || 'steelblue';
                var panelX = marginLeft + (m - 1) * (panelInnerW + panelGutter);
                var panelY = marginTop;

                var g = svg.append('g')
                    .attr('transform', 'translate(' + panelX + ',' + panelY + ')');

                // Clip path so bands don't overflow panel borders
                var clipId = 'ts-dp-clip-' + m;
                g.append('defs').append('clipPath').attr('id', clipId)
                    .append('rect')
                    .attr('width',  panelInnerW)
                    .attr('height', panelInnerH);

                // Light panel background + border
                g.append('rect')
                    .attr('width',  panelInnerW)
                    .attr('height', panelInnerH)
                    .style('fill', '#fafafa')
                    .style('stroke', '#d8d8d8')
                    .style('stroke-width', '0.5px');

                // Month name above panel
                g.append('text')
                    .attr('x', panelInnerW / 2)
                    .attr('y', -6)
                    .style('text-anchor', 'middle')
                    .style('font-size', '13px')
                    .style('font-weight', 'bold')
                    .style('fill', '#333')
                    .text(MONTH_NAMES[m - 1]);

                // Grey all-data min/max band
                var bandAll = HOURS.map(function(h) {
                    return { h: h, min: allStats[m][h].min, max: allStats[m][h].max };
                });
                g.append('path').datum(bandAll).attr('d', areaFn)
                    .attr('clip-path', 'url(#' + clipId + ')')
                    .style('fill', '#d8d8d8')
                    .style('fill-opacity', 0.9);

                // Colored active min/max band + avg line
                var bandActive = HOURS.map(function(h) {
                    return { h: h, min: activeStats[m][h].min, max: activeStats[m][h].max };
                });
                g.append('path').datum(bandActive).attr('d', areaFn)
                    .attr('clip-path', 'url(#' + clipId + ')')
                    .style('fill', color)
                    .style('fill-opacity', 0.35);

                var avgActive = HOURS.map(function(h) {
                    return { h: h, avg: activeStats[m][h].avg };
                });
                g.append('path').datum(avgActive).attr('d', lineFn)
                    .attr('clip-path', 'url(#' + clipId + ')')
                    .style('fill', 'none')
                    .style('stroke', color)
                    .style('stroke-width', '1.5px');

                // X-axis with sparse labels
                var xAxis = d3.svg.axis()
                    .scale(xScale)
                    .orient('bottom')
                    .tickValues(xTickValues)
                    .tickFormat(function(d) { return xTickFmt[d]; });

                g.append('g')
                    .attr('class', 'axis')
                    .attr('transform', 'translate(0,' + panelInnerH + ')')
                    .call(xAxis);

                // Vertical crosshair for this panel
                var panelCrosshair = g.append('line')
                    .attr('y1', 0).attr('y2', panelInnerH)
                    .style('stroke', '#666').style('stroke-width', '1px')
                    .style('stroke-dasharray', '4,3')
                    .style('pointer-events', 'none')
                    .style('display', 'none');

                // Invisible overlay rect to capture mouse events
                g.append('rect')
                    .attr('width', panelInnerW).attr('height', panelInnerH)
                    .style('fill', 'none').style('pointer-events', 'all')
                    .on('mousemove', function() {
                        var mx = d3.mouse(this)[0];
                        var h  = Math.max(1, Math.min(24, Math.round(xScale.invert(mx))));
                        var a  = allStats[m][h];
                        var f  = activeStats[m][h];

                        var html = '<strong>' + MONTH_NAMES[m - 1] + ', ' + hourLabel(h) + '</strong><br>';
                        html += '<span style="color:#999">All &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; '
                            + 'min: ' + (a && a.min !== null ? fmtV2(a.min) : '\u2013') + '&nbsp;'
                            + varDef.unit + ' &nbsp; avg: ' + (a && a.avg !== null ? fmtV2(a.avg) : '\u2013') + '&nbsp;'
                            + varDef.unit + ' &nbsp; max: ' + (a && a.max !== null ? fmtV2(a.max) : '\u2013') + '&nbsp;'
                            + varDef.unit + '</span><br>';
                        html += '<span style="color:steelblue">Filtered '
                            + 'min: ' + (f && f.min !== null ? fmtV2(f.min) : '\u2013') + '&nbsp;'
                            + varDef.unit + ' &nbsp; avg: ' + (f && f.avg !== null ? fmtV2(f.avg) : '\u2013') + '&nbsp;'
                            + varDef.unit + ' &nbsp; max: ' + (f && f.max !== null ? fmtV2(f.max) : '\u2013') + '&nbsp;'
                            + varDef.unit + '</span>';

                        ttip2.html(html)
                            .style('display', 'block')
                            .style('left', (d3.event.clientX + 14) + 'px')
                            .style('top',  (d3.event.clientY - 42) + 'px');

                        panelCrosshair
                            .attr('x1', xScale(h)).attr('x2', xScale(h))
                            .style('display', null);
                    })
                    .on('mouseleave', function() {
                        ttip2.style('display', 'none');
                        panelCrosshair.style('display', 'none');
                    });

            })(mi);
        }
    }

    // ---- Main redraw function ----
    function redrawAll(activeData) {
        var varDef = getVarDef(window._tsCurrentVarId);
        drawAnnualTimeseries(activeData, varDef);
        drawDailyProfiles(activeData, varDef);
    }

    // Initial draw with all data
    redrawAll(allData);

    // ---- Variable dropdown handler ----
    sel.addEventListener('change', function() {
        window._tsCurrentVarId = this.value;
        redrawAll(window._tsLastActive || allData);
    });

    // ---- Crossfilter update hook (called from renderAll in epwValueFilter) ----
    window.updateTimeseries = function(filteredCFData) {
        var activeSet = {};
        filteredCFData.forEach(function(d) { activeSet[d.index] = true; });
        var activeData = allData.filter(function(d) { return activeSet[d.index]; });
        window._tsLastActive = activeData;
        redrawAll(activeData);
    };
}

// ============================================================
// Duration Curve
// ============================================================
function epwDurationCurve(epw) {

  var varDefs = [
    { id: 'dryBulbTemperature',         label: 'Dry Bulb Temperature',         unitIP: '°F',     unitSI: '°C'    },
    { id: 'dewPointTemperature',         label: 'Dew Point Temperature',        unitIP: '°F',     unitSI: '°C'    },
    { id: 'wetBulbTemperature',          label: 'Wet Bulb Temperature',         unitIP: '°F',     unitSI: '°C'    },
    { id: 'relativeHumidity',           label: 'Relative Humidity',            unitIP: '%',      unitSI: '%'     },
    { id: 'humidityRatio',              label: 'Humidity Ratio',               unitIP: 'lb/lb',  unitSI: 'kg/kg' },
    { id: 'enthalpy',                   label: 'Enthalpy',                     unitIP: 'Btu/lb', unitSI: 'kJ/kg' },
    { id: 'globalHorizontalRadiation',  label: 'Global Horizontal Radiation',  unitIP: 'Wh/m²',  unitSI: 'Wh/m²' },
    { id: 'directNormalRadiation',      label: 'Direct Normal Radiation',      unitIP: 'Wh/m²',  unitSI: 'Wh/m²' },
    { id: 'diffuseHorizontalRadiation', label: 'Diffuse Horizontal Radiation', unitIP: 'Wh/m²',  unitSI: 'Wh/m²' },
    { id: 'windSpeed',                  label: 'Wind Speed',                   unitIP: 'mph',    unitSI: 'm/s'   },
    { id: 'windDirection',              label: 'Wind Direction',               unitIP: '°',   unitSI: '°'  },
    { id: 'totalSkyCover',              label: 'Total Sky Cover',              unitIP: 'tenths', unitSI: 'tenths' },
    { id: 'opaqueSkyCover',             label: 'Opaque Sky Cover',             unitIP: 'tenths', unitSI: 'tenths' },
    { id: 'visibility',                 label: 'Visibility',                   unitIP: 'km',     unitSI: 'km'     },
    { id: 'ceilingHeight',              label: 'Ceiling Height',               unitIP: 'ft',     unitSI: 'm'      },
    { id: 'precipitableWater',          label: 'Precipitable Water',           unitIP: 'mm',     unitSI: 'mm'     },
    { id: 'aerosolOpticalDepth',        label: 'Aerosol Optical Depth',        unitIP: '',       unitSI: ''       },
    { id: 'snowDepth',                  label: 'Snow Depth',                   unitIP: 'in',     unitSI: 'cm'     },
    { id: 'daysSinceLastSnowfall',      label: 'Days Since Last Snowfall',     unitIP: 'days',   unitSI: 'days'   },
    { id: 'albedo',                     label: 'Albedo',                       unitIP: '',       unitSI: ''       },
    { id: 'liquidPrecipitationDepth',   label: 'Liquid Precip. Depth',         unitIP: 'mm',     unitSI: 'mm'     },
    { id: 'liquidPrecipitationQuantity',label: 'Liquid Precip. Quantity',      unitIP: 'hr',     unitSI: 'hr'     },
    { id: 'pressure',                   label: 'Pressure',                     unitIP: 'psi',    unitSI: 'Pa'     },
    { id: 'pvAC',                       label: 'PV AC Output',                 unitIP: 'kWh',    unitSI: 'kWh',
      fmt: function(v) { return Number(v).toPrecision(2); } }
  ];

  var refDefs = [
    { key: 'max', label: 'Max', frac: 0    },
    { key: 'p99', label: '99%', frac: 0.01 },
    { key: 'p95', label: '95%', frac: 0.05 },
    { key: 'p75', label: '75%', frac: 0.25 },
    { key: 'p50', label: '50%', frac: 0.50 },
    { key: 'p25', label: '25%', frac: 0.75 },
    { key: 'p05', label: '5%',  frac: 0.95 },
    { key: 'p01', label: '1%',  frac: 0.99 },
    { key: 'min', label: 'Min', frac: 1    }
  ];

  if (!window._durRefState) {
    window._durRefState = {};
    var durDefaultOn = { max: true, p99: true, p50: true, p01: true, min: true };
    refDefs.forEach(function(r) { window._durRefState[r.key] = durDefaultOn[r.key] || false; });
  }

  // Populate variable select
  var sel = document.getElementById('durVariableSelect');
  if (sel.options.length === 0) {
    varDefs.forEach(function(v) {
      var opt = document.createElement('option');
      opt.value = v.id;
      opt.textContent = v.label;
      sel.appendChild(opt);
    });
  }
  if (window._durCurrentVarId) {
    sel.value = window._durCurrentVarId;
  } else {
    window._durCurrentVarId = sel.value;
  }

  // Build ref line checkbox panel once
  var panel = document.getElementById('durRefLinesPanel');
  if (!panel.__built) {
    panel.__built = true;
    var titleEl = document.createElement('div');
    titleEl.className = 'dur-ref-title';
    titleEl.textContent = 'Ref Lines';
    panel.appendChild(titleEl);
    refDefs.forEach(function(r) {
      var lbl = document.createElement('label');
      lbl.className = 'dur-refline-item';
      var cb = document.createElement('input');
      cb.type = 'checkbox';
      cb.id = 'durRef_' + r.key;
      cb.checked = window._durRefState[r.key];
      cb.addEventListener('change', function() {
        window._durRefState[r.key] = cb.checked;
        d3.select('#epwDurationChart svg').select('.dur-refline-' + r.key)
          .style('display', cb.checked ? '' : 'none');
      });
      lbl.appendChild(cb);
      lbl.appendChild(document.createTextNode(' ' + r.label));
      panel.appendChild(lbl);
    });
  }

  var allData = window._tsAllData;
  if (!allData || allData.length === 0) { return; }

  var margin = { top: 24, right: 165, bottom: 60, left: 70 };
  var svgW = 880, svgH = 464;
  var W = svgW - margin.left - margin.right;
  var H = svgH - margin.top  - margin.bottom;

  function getUnit(v) {
    return (unitSystem === 'IP') ? v.unitIP : v.unitSI;
  }

  function draw(activeData) {
    var container = d3.select('#epwDurationChart');
    container.select('svg').remove();

    var varId = window._durCurrentVarId;
    var vDef  = varDefs.filter(function(v) { return v.id === varId; })[0] || varDefs[0];

    // Sort descending — full dataset for background, active for foreground
    var allSorted = allData.slice().sort(function(a, b) { return b[varId] - a[varId]; });
    var sorted    = activeData.slice().sort(function(a, b) { return b[varId] - a[varId]; });
    var allN = allSorted.length;
    var N    = sorted.length;

    // x-axis always spans the full dataset length for stable context
    var xScale = d3.scale.linear().domain([0, Math.max(allN - 1, 1)]).range([0, W]);
    var yExt   = d3.extent(allSorted, function(d) { return d[varId]; });
    var yPad   = ((yExt[1] - yExt[0]) * 0.05) || 1;
    var yScale = d3.scale.linear().domain([yExt[0] - yPad, yExt[1] + yPad]).range([H, 0]);

    var color = window._plotColor || 'steelblue';

    var svg = container.append('svg')
        .attr('width', svgW)
        .attr('height', svgH)
      .append('g')
        .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');

    svg.append('defs').append('clipPath')
        .attr('id', 'durClip')
      .append('rect')
        .attr('width', W)
        .attr('height', H);

    // Axes
    var xAxis = d3.svg.axis().scale(xScale).orient('bottom').ticks(8);
    var yAxis = d3.svg.axis().scale(yScale).orient('left').ticks(8);

    svg.append('g')
        .attr('class', 'x axis')
        .attr('transform', 'translate(0,' + H + ')')
        .call(xAxis)
      .append('text')
        .attr('x', W / 2)
        .attr('y', 48)
        .attr('fill', '#333')
        .style('font-size', '13px')
        .style('text-anchor', 'middle')
        .text('Hours (ranked)');

    svg.append('g')
        .attr('class', 'y axis')
        .call(yAxis)
      .append('text')
        .attr('transform', 'rotate(-90)')
        .attr('x', -H / 2)
        .attr('y', -55)
        .attr('fill', '#333')
        .style('font-size', '13px')
        .style('text-anchor', 'middle')
        .text(vDef.label + ' (' + getUnit(vDef) + ')');

    var chartArea = svg.append('g').attr('clip-path', 'url(#durClip)');

    if (allN > 0) {
      var line = d3.svg.line()
          .x(function(d, i) { return xScale(i); })
          .y(function(d)    { return yScale(d[varId]); });

      // Grey background line — full unfiltered dataset
      chartArea.append('path')
          .datum(allSorted)
          .attr('fill', 'none')
          .attr('stroke', '#bbb')
          .attr('stroke-width', 1.5)
          .attr('d', line);

      // Colored foreground line — filtered dataset
      if (N > 0) {
        chartArea.append('path')
            .datum(sorted)
            .attr('fill', 'none')
            .attr('stroke', color)
            .attr('stroke-width', 1.5)
            .attr('d', line);
      }
    }

    // Reference lines (based on full dataset for stable context)
    refDefs.forEach(function(r) {
      var rankIdx = (allN > 0) ? Math.min(Math.round(r.frac * (allN - 1)), allN - 1) : 0;
      var val = (allN > 0) ? allSorted[rankIdx][varId] : NaN;

      var grp = svg.append('g')
          .attr('class', 'dur-refline-' + r.key)
          .style('display', (allN > 0 && window._durRefState[r.key]) ? '' : 'none');

      grp.append('line')
          .attr('x1', 0).attr('x2', W)
          .attr('y1', yScale(val)).attr('y2', yScale(val))
          .attr('stroke', '#555')
          .attr('stroke-width', 1)
          .attr('stroke-dasharray', '3,2');

      grp.append('text')
          .attr('x', W + 4)
          .attr('y', yScale(val) + 4)
          .attr('fill', '#555')
          .style('font-size', '11px')
          .text(r.label + ': ' + (isNaN(val) ? '-' : (vDef.fmt ? vDef.fmt(val) : val.toFixed(1)) + ' ' + getUnit(vDef)));
    });

    // Hover crosshair + tooltip
    var tooltip  = d3.select('#epw-dur-tooltip');
    var focusLine = svg.append('line')
        .attr('y1', 0).attr('y2', H)
        .attr('stroke', '#aaa')
        .attr('stroke-width', 1)
        .attr('stroke-dasharray', '3,2')
        .style('display', 'none');

    svg.append('rect')
        .attr('width', W)
        .attr('height', H)
        .attr('fill', 'none')
        .attr('pointer-events', 'all')
        .on('mousemove', function() {
          if (allN === 0) { return; }
          var mx   = d3.mouse(this)[0];
          var rank = Math.max(0, Math.min(Math.round(xScale.invert(mx)), allN - 1));
          var d    = allSorted[rank];
          focusLine.style('display', '').attr('x1', mx).attr('x2', mx);
          tooltip
              .style('display', 'block')
              .style('left', (d3.event.clientX + 14) + 'px')
              .style('top',  (d3.event.clientY - 28) + 'px')
              .html('Rank: ' + (rank + 1) + ' of ' + allN + '<br/>' +
                    vDef.label + ': <b>' + (vDef.fmt ? vDef.fmt(d[varId]) : d[varId].toFixed(2)) + ' ' + getUnit(vDef) + '</b>');
        })
        .on('mouseout', function() {
          focusLine.style('display', 'none');
          tooltip.style('display', 'none');
        });
  }

  draw(window._durLastActive || allData);

  d3.select('#durVariableSelect').on('change', function() {
    window._durCurrentVarId = this.value;
    draw(window._durLastActive || allData);
  });

  window.updateDurationCurve = function(filteredCFData) {
    var activeSet = {};
    filteredCFData.forEach(function(d) { activeSet[d.index] = true; });
    var active = allData.filter(function(d) { return activeSet[d.index]; });
    window._durLastActive = active;
    draw(active);
  };
  // Expose allData ref so PV Recalculate can patch pvAC values before updateDurationCurve runs
  window._durAllData = allData;
}

// ============================================================
// Degree Days
// ============================================================
function epwDegreeDays(epw) {

  var allData = window._tsAllData;
  if (!allData || allData.length === 0) { return; }

  var monthNames = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];

  var isIP = (unitSystem === 'IP');
  var defaultBase = isIP ? 65 : 18;
  var unitLabel   = isIP ? '\u00b0F' : '\u00b0C';

  // Set unit labels in UI
  document.getElementById('ddHeatUnit').textContent = unitLabel;
  document.getElementById('ddCoolUnit').textContent = unitLabel;

  // Reset input defaults when unit system changes
  var heatInput = document.getElementById('ddHeatBase');
  var coolInput = document.getElementById('ddCoolBase');
  if (!window._ddUnitSystem || window._ddUnitSystem !== unitSystem) {
    window._ddUnitSystem = unitSystem;
    window._ddHeatBase = defaultBase;
    window._ddCoolBase = defaultBase;
    heatInput.value = defaultBase;
    coolInput.value = defaultBase;
  }

  // Compute monthly HDD + CDD inline from dry-bulb temperature
  function calcDD(data, hBase, cBase) {
    var months = d3.range(0, 12).map(function() { return { hdd: 0, cdd: 0 }; });
    data.forEach(function(d) {
      months[d.month - 1].hdd += Math.max(0, hBase - d.dryBulbTemperature) / 24;
      months[d.month - 1].cdd += Math.max(0, d.dryBulbTemperature - cBase) / 24;
    });
    return months;
  }

  var margin = { top: 20, right: 30, bottom: 50, left: 75 };
  var svgW = 820, svgH = 380;
  var W = svgW - margin.left - margin.right;
  var H = svgH - margin.top  - margin.bottom;

  var HDD_FILT   = '#c0392b';
  var HDD_ALL    = '#e8a9a0';
  var CDD_FILT   = '#1a6faf';
  var CDD_ALL    = '#a0c4e8';

  function draw(activeData) {
    var container = d3.select('#epwDegreeDaysChart');
    container.select('svg').remove();

    var hBase  = window._ddHeatBase || defaultBase;
    var cBase  = window._ddCoolBase || defaultBase;
    var uLabel = isIP ? '\u00b0F' : '\u00b0C';

    var allDD    = calcDD(allData, hBase, cBase);
    var activeDD = calcDD(activeData, hBase, cBase);

    // Update annual totals banner
    var totalHDD = d3.sum(activeDD, function(d) { return d.hdd; });
    var totalCDD = d3.sum(activeDD, function(d) { return d.cdd; });
    document.getElementById('ddTotals').innerHTML =
      '<span class="dd-hdd">Heating Degree Days (' + hBase + uLabel + '): ' + Math.round(totalHDD) + '</span>' +
      '&nbsp;&nbsp;&nbsp;&nbsp;' +
      '<span class="dd-cdd">Cooling Degree Days (' + cBase + uLabel + '): ' + Math.round(totalCDD) + '</span>';

    // Scales
    var x0 = d3.scale.ordinal().domain(monthNames).rangeRoundBands([0, W], 0.18);
    var x1 = d3.scale.ordinal().domain(['hdd', 'cdd']).rangeRoundBands([0, x0.rangeBand()], 0.06);

    var yMax = d3.max(allDD, function(d) { return Math.max(d.hdd, d.cdd); }) || 1;
    var yScale = d3.scale.linear().domain([0, yMax * 1.06]).range([H, 0]);

    var svg = container.append('svg')
        .attr('width', svgW)
        .attr('height', svgH)
      .append('g')
        .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');

    var tooltip = d3.select('#epw-dd-tooltip');

    // Draw grouped bars for each month
    monthNames.forEach(function(mName, mi) {
      var allRow    = allDD[mi];
      var activeRow = activeDD[mi];
      var xBase = x0(mName);

      [
        { key: 'hdd', allVal: allRow.hdd,  filtVal: activeRow.hdd,  colorAll: HDD_ALL,  colorFilt: HDD_FILT,  label: 'HDD' },
        { key: 'cdd', allVal: allRow.cdd,  filtVal: activeRow.cdd,  colorAll: CDD_ALL,  colorFilt: CDD_FILT,  label: 'CDD' }
      ].forEach(function(bar) {
        var bx = xBase + x1(bar.key);
        var bw = x1.rangeBand();

        // Background bar — all data
        if (bar.allVal > 0) {
          svg.append('rect')
              .attr('x', bx).attr('y', yScale(bar.allVal))
              .attr('width', bw).attr('height', H - yScale(bar.allVal))
              .attr('fill', bar.colorAll);
        }

        // Foreground bar — filtered, with tooltip
        if (bar.filtVal > 0) {
          svg.append('rect')
              .attr('x', bx).attr('y', yScale(bar.filtVal))
              .attr('width', bw).attr('height', H - yScale(bar.filtVal))
              .attr('fill', bar.colorFilt)
              .style('cursor', 'default')
              .on('mouseover', function() { tooltip.style('display', 'block'); })
              .on('mousemove', function() {
                var extra = (Math.abs(bar.allVal - bar.filtVal) > 0.05)
                  ? '<br/><span style="color:#aaa">All data: ' + bar.allVal.toFixed(1) + '</span>'
                  : '';
                tooltip
                    .style('left', (d3.event.clientX + 14) + 'px')
                    .style('top',  (d3.event.clientY - 28) + 'px')
                    .html(mName + ' <b>' + bar.label + '</b>: ' + bar.filtVal.toFixed(1) + ' degree-days' + extra);
              })
              .on('mouseout',  function() { tooltip.style('display', 'none'); });
        }
      });
    });

    // Axes
    var xAxis = d3.svg.axis().scale(x0).orient('bottom');
    var yAxis = d3.svg.axis().scale(yScale).orient('left').ticks(6);

    svg.append('g')
        .attr('class', 'x axis')
        .attr('transform', 'translate(0,' + H + ')')
        .call(xAxis);

    svg.append('g')
        .attr('class', 'y axis')
        .call(yAxis)
      .append('text')
        .attr('transform', 'rotate(-90)')
        .attr('x', -H / 2)
        .attr('y', -60)
        .attr('fill', '#333')
        .style('font-size', '13px')
        .style('text-anchor', 'middle')
        .text('Degree-Days');
  }

  draw(window._ddLastActive || allData);

  // Re-draw when base temps are changed
  heatInput.addEventListener('change', function() {
    window._ddHeatBase = parseFloat(this.value) || defaultBase;
    draw(window._ddLastActive || allData);
  });
  coolInput.addEventListener('change', function() {
    window._ddCoolBase = parseFloat(this.value) || defaultBase;
    draw(window._ddLastActive || allData);
  });

  window.updateDegreeDays = function(filteredCFData) {
    var activeSet = {};
    filteredCFData.forEach(function(d) { activeSet[d.index] = true; });
    var active = allData.filter(function(d) { return activeSet[d.index]; });
    window._ddLastActive = active;
    draw(active);
  };
}

/*-------------------------------------------------------------------------
 * epwSolarPV
 * Monthly PV production bar chart using the SolarPV.js calculation library.
 * Not responsive to crossfilter value filters — computed once per call.
 *-------------------------------------------------------------------------*/
function epwSolarPV(epw) {
  if (typeof SolarPV === 'undefined') {
    console.warn('epwSolarPV: solarPV.js not loaded — skipping PV chart.');
    return;
  }

  var monthNames = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];

  // Read user-input parameters from the form controls
  function getParams() {
    var dc  = parseFloat(document.getElementById('pvDCCapacity').value);
    var mod = parseInt(document.getElementById('pvModuleType').value, 10);
    var arr = parseInt(document.getElementById('pvArrayType').value, 10);
    var lss = parseFloat(document.getElementById('pvSystemLosses').value);
    var tlt = parseFloat(document.getElementById('pvTilt').value);
    var azm = parseFloat(document.getElementById('pvAzimuth').value);
    if (!isFinite(dc)  || dc  <= 0)   dc  = 1.0;
    if (!isFinite(lss) || lss < 0)    lss = 14;
    if (!isFinite(tlt) || tlt < 0)    tlt = 10;
    if (!isFinite(azm))                azm = 180;
    if (isNaN(mod) || mod < 0 || mod > 2) mod = 1;
    if (isNaN(arr) || arr < 0 || arr > 4) arr = 1;
    return {
      dcCapacity_kW : dc,
      moduleType    : mod,
      arrayType     : arr,
      systemLosses  : lss / 100,
      tilt_deg      : tlt,
      azimuth_deg   : azm
    };
  }

  function draw() {
    var params  = getParams();

    // Run PVWatts calculation
    var result     = SolarPV.computeAnnualPV(epw, params);
    var monthly    = result.monthly;
    var annual     = result.annual;
    var monthlyPOA = result.monthlyPOA;
    var annualPOA  = result.annualPOA;

    // Derived metrics
    // Capacity Factor: fraction of time the system runs at nameplate output
    var cf = (annual / (params.dcCapacity_kW * 8760) * 100).toFixed(1); // %
    // Performance Ratio: AC energy delivered vs. ideal energy at actual POA
    // PR = E_AC / (H_POA × P_STC / G_STC) where G_STC = 1 kW/m²
    var pr = (annual / (params.dcCapacity_kW * annualPOA)).toFixed(3);  // dimensionless

    // Persist for CSV download
    window._pvLastResult = result;
    window._pvLastParams = params;

    // Store hourly AC (W) for cross-tab integration (crossfilter, timeseries, duration, scatter, heatmap)
    window._pvHourlyAC = result.hourlyAC;
    var _pvMax = Math.max.apply(null, result.hourlyAC);
    console.log('[epwSolarPV] hourlyAC computed: n=' + result.hourlyAC.length + ', max=' + _pvMax.toFixed(1) + ' W, annual=' + Math.round(result.annual) + ' kWh');
    // Patch _tsAllData so timeseries and duration curve tabs reflect current PV params
    if (window._tsAllData) {
      var _hAC = result.hourlyAC;
      for (var _k = 0; _k < window._tsAllData.length && _k < _hAC.length; _k++) {
        window._tsAllData[_k].pvAC = (_hAC[_k] || 0) / 1000;
      }
    }
    // Patch duration curve allData
    if (window._durAllData) {
      var _hAC2 = result.hourlyAC;
      for (var _k2 = 0; _k2 < window._durAllData.length && _k2 < _hAC2.length; _k2++) {
        window._durAllData[_k2].pvAC = (_hAC2[_k2] || 0) / 1000;
      }
    }
    // Patch tabular allData
    if (window._tblAllData) {
      var _hAC3 = result.hourlyAC;
      for (var _k3 = 0; _k3 < window._tblAllData.length && _k3 < _hAC3.length; _k3++) {
        window._tblAllData[_k3].pvAC = (_hAC3[_k3] || 0) / 1000;
      }
    }

    // Annual-total banner
    var annualEl = document.getElementById('pvAnnualTotal');
    if (annualEl) {
      annualEl.innerHTML =
        'Annual AC Production: <b>' + Math.round(annual) + ' kWh</b>' +
        ' &nbsp;|  Specific Yield: <b>' + Math.round(annual / params.dcCapacity_kW) + ' kWh / kW<sub>DC</sub></b>' +
        ' &nbsp;|  Capacity Factor: <b>' + cf + '%</b>' +
        ' &nbsp;|  Performance Ratio: <b>' + pr + '</b>' +
        ' &nbsp;|  Annual POA: <b>' + Math.round(annualPOA) + ' kWh / m²</b>';
    }

    // Annual-total banner — note: terminology moved to static attribution block

    // ---- D3 bar chart ----
    var container = d3.select('#epwSolarPVChart');
    container.select('svg').remove();

    var margin = { top: 30, right: 30, bottom: 50, left: 80 };
    var svgW = 820, svgH = 380;
    var W = svgW - margin.left - margin.right;
    var H = svgH - margin.top  - margin.bottom;

    var svg = container.append('svg')
        .attr('width',  svgW)
        .attr('height', svgH)
      .append('g')
        .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');

    // Chart title
    svg.append('text')
        .attr('x', 0)
        .attr('y', -margin.top / 2)
        .attr('text-anchor', 'start')
        .style('font-size', '13px')
        .style('font-weight', 'bold')
        .style('fill', '#333')
        .text('Monthly PV Production (kWh)');

    var xScale = d3.scale.ordinal()
        .domain(monthNames)
        .rangeRoundBands([0, W], 0.2);

    var yMax = (Math.max.apply(null, monthly) || 1) * 1.18;
    var yScale = d3.scale.linear()
        .domain([0, yMax])
        .range([H, 0]);

    var BAR_COLOR = '#f5c400';

    var tooltip = d3.select('#epw-pv-tooltip');

    // Bars + labels
    monthNames.forEach(function (mName, mi) {
      var val = monthly[mi];
      var bx  = xScale(mName);
      var bw  = xScale.rangeBand();
      var by  = yScale(val);
      var bh  = H - by;

      if (val > 0) {
        svg.append('rect')
            .attr('x', bx).attr('y', by)
            .attr('width', bw).attr('height', bh)
            .attr('fill', BAR_COLOR)
            .style('cursor', 'default')
            .on('mouseover', function () { tooltip.style('display', 'block'); })
            .on('mousemove', function () {
              tooltip
                  .style('left', (d3.event.clientX + 14) + 'px')
                  .style('top',  (d3.event.clientY - 28) + 'px')
                  .html(mName + ': <b>' + val.toFixed(1) + ' kWh</b>');
            })
            .on('mouseout', function () { tooltip.style('display', 'none'); });

        // Value label above bar
        svg.append('text')
            .attr('x', bx + bw / 2)
            .attr('y', by - 5)
            .attr('text-anchor', 'middle')
            .style('font-size', '11px')
            .style('fill', '#333')
            .text(Math.round(val));
      }
    });

    // X axis
    svg.append('g')
        .attr('class', 'x axis')
        .attr('transform', 'translate(0,' + H + ')')
        .call(d3.svg.axis().scale(xScale).orient('bottom'));

    // Y axis
    svg.append('g')
        .attr('class', 'y axis')
        .call(d3.svg.axis().scale(yScale).orient('left').ticks(6));

    // ---- Secondary chart: POA irradiance ---------------------------------
    var poaContainer = d3.select('#epwSolarPOAChart');
    poaContainer.select('svg').remove();

    var svgPOA = poaContainer.append('svg')
        .attr('width',  svgW)
        .attr('height', svgH)
      .append('g')
        .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')');

    // Chart title
    svgPOA.append('text')
        .attr('x', 0)
        .attr('y', -margin.top / 2)
        .attr('text-anchor', 'start')
        .style('font-size', '13px')
        .style('font-weight', 'bold')
        .style('fill', '#333')
        .text('Plane-of-Array (POA) Irradiance (kWh/m²)');

    var yMaxPOA   = (Math.max.apply(null, monthlyPOA) || 1) * 1.18;
    var yScalePOA = d3.scale.linear()
        .domain([0, yMaxPOA])
        .range([H, 0]);

    var POA_COLOR = window._plotColor || 'steelblue';

    monthNames.forEach(function (mName, mi) {
      var val = monthlyPOA[mi];
      var bx  = xScale(mName);
      var bw  = xScale.rangeBand();
      var by  = yScalePOA(val);
      var bh  = H - by;

      if (val > 0) {
        svgPOA.append('rect')
            .attr('x', bx).attr('y', by)
            .attr('width', bw).attr('height', bh)
            .attr('fill', POA_COLOR)
            .style('cursor', 'default')
            .on('mouseover', function () { tooltip.style('display', 'block'); })
            .on('mousemove', (function (mn, v) {
              return function () {
                tooltip
                    .style('left', (d3.event.clientX + 14) + 'px')
                    .style('top',  (d3.event.clientY - 28) + 'px')
                    .html(mn + ': <b>' + v.toFixed(1) + ' kWh/m²</b>');
              };
            }(mName, val)))
            .on('mouseout', function () { tooltip.style('display', 'none'); });

        svgPOA.append('text')
            .attr('x', bx + bw / 2)
            .attr('y', by - 5)
            .attr('text-anchor', 'middle')
            .style('font-size', '11px')
            .style('fill', '#333')
            .text(Math.round(val));
      }
    });

    // X axis
    svgPOA.append('g')
        .attr('class', 'x axis')
        .attr('transform', 'translate(0,' + H + ')')
        .call(d3.svg.axis().scale(xScale).orient('bottom'));

    // Y axis
    svgPOA.append('g')
        .attr('class', 'y axis')
        .call(d3.svg.axis().scale(yScalePOA).orient('left').ticks(6));
  }

  // Initial render
  draw();

  // Wire up Recalculate button
  var btn = document.getElementById('pvRecalcBtn');
  if (btn) {
    // Replace previous handler to avoid stacking listeners on unit-system toggle
    btn.onclick = function () {
      draw();
      // Rebuild crossfilter: updates pvAC_kWh data, resets pv-ac x-axis, triggers renderAll
      // renderAll calls updateTimeseries, updateDurationCurve, updateTabular, updateScatter
      if (window._pvRebuildValueFilter) window._pvRebuildValueFilter();
      // Force scatter to recompute dot positions (getVal reads _pvHourlyAC live)
      if (window._scatterFullRedraw) window._scatterFullRedraw();
      // Re-render heatmap with updated pvACOutput data then reapply filter state
      if (window._hmRerenderAll) window._hmRerenderAll();
    };
  }
}
