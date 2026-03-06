/*-------------------------------------------------------------------------
 * epw2json.js
 * Parses an epw file into a JSON object
 *-------------------------------------------------------------------------*/
function epw2json(raw) {
  //read raw file
  var epw_raw = d3.csv.parseRows(raw);

  //See the EPW IDD specification for epw file format
  var epw = {
      _location : {},
      designCondition : {},
      designConditions : {},
      typicalExtremePeriod : {},
      typicalExtremePeriods : {},
      groundTemperature : {},
      groundTemperatures : {},
      holiday : {},
      holidayDaylightSavings : {},
      comments1 : {},
      comments2 : {},
      dataPeriod : {},
      dataPeriods : {},
      weatherData : []
  };

  //epw object functions for getting and setting data
  epw.getStationLocation = function() {
    return this.stationLocation;
  };
  epw.setStationLocation = function(stationLocation) {
    this.stationLocation = stationLocation;
  };

  //general method for getting data by field number in weather data
  epw.getDataByField = function(fieldNumber) {
    data = [];
    //field 5 is uncertainty data and is not a number
    if (fieldNumber == 5) {
      this.weatherData.forEach(function(row) {
        data.push(row[fieldNumber]);
      });
    } else {
      this.weatherData.forEach(function(row) {
        data.push(+row[fieldNumber]);
      });
    };
    return data;
  };

  epw.getPsychrometricField = function(fieldName) {
    if (!this.psychrometrics || !this.psychrometrics[fieldName]) {
      this.computePsychrometrics();
    }
    if (!this.psychrometrics || !this.psychrometrics[fieldName]) {
      return [];
    }
    return this.psychrometrics[fieldName].slice(0);
  };

  epw.computePsychrometrics = function() {
    var properties = {
      wetBulbTemperature: [],
      humidityRatio: [],
      specificHumidity: [],
      vaporPressure: [],
      moistAirEnthalpy: [],
      moistAirVolume: [],
      degreeOfSaturation: [],
      moistAirDensity: []
    };

    if (typeof psychrolib === "undefined" || !psychrolib || typeof psychrolib.SetUnitSystem !== "function") {
      this.psychrometrics = properties;
      return properties;
    }

    psychrolib.SetUnitSystem(psychrolib.SI);

    var minDryBulbTemperatureC = -100;
    var maxDryBulbTemperatureC = 70;
    var minRelativeHumidityPercent = 0;
    var maxRelativeHumidityPercent = 100;
    var minPressurePa = 10000;
    var maxPressurePa = 120000;

    this.weatherData.forEach(function(row) {
      var dryBulbTemperature = +row[6];
      var relativeHumidity = +row[8];
      var pressure = +row[9];

      var invalid = !isFinite(dryBulbTemperature) || !isFinite(relativeHumidity) || !isFinite(pressure)
        || dryBulbTemperature > maxDryBulbTemperatureC || dryBulbTemperature < minDryBulbTemperatureC
        || relativeHumidity < minRelativeHumidityPercent || relativeHumidity > maxRelativeHumidityPercent
        || pressure < minPressurePa || pressure > maxPressurePa;

      if (invalid) {
        properties.wetBulbTemperature.push(null);
        properties.humidityRatio.push(null);
        properties.specificHumidity.push(null);
        properties.vaporPressure.push(null);
        properties.moistAirEnthalpy.push(null);
        properties.moistAirVolume.push(null);
        properties.degreeOfSaturation.push(null);
        properties.moistAirDensity.push(null);
        return;
      }

      try {
        var relHum = relativeHumidity / 100;
        var psychrometrics = psychrolib.CalcPsychrometricsFromRelHum(dryBulbTemperature, relHum, pressure);
        var humidityRatio = psychrometrics[0];

        properties.wetBulbTemperature.push(psychrometrics[1]);
        properties.humidityRatio.push(humidityRatio);
        properties.specificHumidity.push(psychrolib.GetSpecificHumFromHumRatio(humidityRatio));
        properties.vaporPressure.push(psychrometrics[3]);
        properties.moistAirEnthalpy.push(psychrometrics[4]);
        properties.moistAirVolume.push(psychrometrics[5]);
        properties.degreeOfSaturation.push(psychrometrics[6]);
        properties.moistAirDensity.push(psychrolib.GetMoistAirDensity(dryBulbTemperature, humidityRatio, pressure));
      } catch (err) {
        properties.wetBulbTemperature.push(null);
        properties.humidityRatio.push(null);
        properties.specificHumidity.push(null);
        properties.vaporPressure.push(null);
        properties.moistAirEnthalpy.push(null);
        properties.moistAirVolume.push(null);
        properties.degreeOfSaturation.push(null);
        properties.moistAirDensity.push(null);
      }
    });

    this.psychrometrics = properties;
    return properties;
  };

  // Data fields in weather data (EPW IDD field numbers, 0-indexed after header rows are stripped)

  // Field 0 | Year
  epw.year = function() {
    data = this.getDataByField(0);
    return data;
  };
  // Field 1 | Month | 1–12
  epw.month = function() {
    data = this.getDataByField(1);
    return data;
  };
  // Field 2 | Day of month | 1–31
  epw.day = function() {
    data = this.getDataByField(2);
    return data;
  };
  // Field 3 | Hour | 1–24 (1 = first hour of the day, i.e. 00:01–01:00)
  epw.hour = function() {
    data = this.getDataByField(3);
    return data;
  };
  // Field 4 | Minute | 0, 15, 30, or 45
  epw.minute = function() {
    data = this.getDataByField(4);
    return data;
  };
  // Field 5 | Data Source and Uncertainty Flags | string code (see EPW IDD)
  epw.uncertainty = function() {
    data = this.getDataByField(5);
    return data;
  };
  // Field 6 | Dry Bulb Temperature | °C | range: -70 to 70 | missing: 99.9
  epw.dryBulbTemperature = function() {
    data = this.getDataByField(6);
    return data;
  };
  // Field 7 | Dew Point Temperature | °C | range: -70 to 70 | missing: 99.9
  epw.dewPointTemperature = function() {
    data = this.getDataByField(7);
    return data;
  };
  // Field 8 | Relative Humidity | % | range: 0–110 | missing: 999
  epw.relativeHumidity = function() {
    data = this.getDataByField(8);
    return data;
  };
  // --- Psychrometric properties derived via PsychroLib (SI units, computed from fields 6, 8, 9) ---

  // Wet Bulb Temperature | °C | computed from dry-bulb, RH, and pressure
  epw.wetBulbTemperature = function() {
    data = this.getPsychrometricField("wetBulbTemperature");
    return data;
  };
  // Humidity Ratio (mixing ratio) | kg water / kg dry air | computed
  epw.humidityRatio = function() {
    data = this.getPsychrometricField("humidityRatio");
    return data;
  };
  // Specific Humidity | kg water / kg moist air | computed
  epw.specificHumidity = function() {
    data = this.getPsychrometricField("specificHumidity");
    return data;
  };
  // Partial Pressure of Water Vapor | Pa | computed
  epw.vaporPressure = function() {
    data = this.getPsychrometricField("vaporPressure");
    return data;
  };
  // Moist Air Enthalpy | J / kg dry air | computed
  epw.moistAirEnthalpy = function() {
    data = this.getPsychrometricField("moistAirEnthalpy");
    return data;
  };
  // Moist Air Specific Volume | m³ / kg dry air | computed
  epw.moistAirVolume = function() {
    data = this.getPsychrometricField("moistAirVolume");
    return data;
  };
  // Degree of Saturation | dimensionless ratio (0–1) | computed
  epw.degreeOfSaturation = function() {
    data = this.getPsychrometricField("degreeOfSaturation");
    return data;
  };
  // Moist Air Density | kg / m³ | computed
  epw.moistAirDensity = function() {
    data = this.getPsychrometricField("moistAirDensity");
    return data;
  };

  // --- Remaining EPW IDD fields ---

  // Field 9 | Atmospheric Station Pressure | Pa | range: 31000–120000 | missing: 999999
  epw.atmosphericStationPressure = function() {
    data = this.getDataByField(9);
    return data;
  };
  // Field 10 | Extraterrestrial Horizontal Radiation | Wh/m² | range: 0–9999 | missing: 9999
  epw.extraterrestrialHorizontalRadiation = function() {
    data = this.getDataByField(10);
    return data;
  };
  // Field 11 | Extraterrestrial Direct Normal Radiation | Wh/m² | range: 0–9999 | missing: 9999
  epw.extraterrestrialDirectNormalRadiation = function() {
    data = this.getDataByField(11);
    return data;
  };
  // Field 12 | Horizontal Infrared Radiation Intensity (sky) | Wh/m² | range: 0–9999 | missing: 9999
  epw.horizontalInfraredRadiationIntensity = function() {
    data = this.getDataByField(12);
    return data;
  };
  // Field 13 | Global Horizontal Radiation | Wh/m² | range: 0–9999 | missing: 9999
  epw.globalHorizontalRadiation = function() {
    data = this.getDataByField(13);
    return data;
  };
  // Field 14 | Direct Normal Radiation | Wh/m² | range: 0–9999 | missing: 9999
  epw.directNormalRadiation = function() {
    data = this.getDataByField(14);
    return data;
  };
  // Field 15 | Diffuse Horizontal Radiation | Wh/m² | range: 0–9999 | missing: 9999
  epw.diffuseHorizontalRadiation = function() {
    data = this.getDataByField(15);
    return data;
  };
  // Field 16 | Global Horizontal Illuminance | lux | range: 0–999999 | missing: 999999
  epw.globalHorizontalIlluminance = function() {
    data = this.getDataByField(16);
    return data;
  };
  // Field 17 | Direct Normal Illuminance | lux | range: 0–999999 | missing: 999999
  epw.directNormalIlluminance = function() {
    data = this.getDataByField(17);
    return data;
  };
  // Field 18 | Diffuse Horizontal Illuminance | lux | range: 0–999999 | missing: 999999
  epw.diffuseHorizontalIlluminance = function() {
    data = this.getDataByField(18);
    return data;
  };
  // Field 19 | Zenith Luminance | cd/m² | range: 0–9999 | missing: 9999
  epw.zenithLuminance = function() {
    data = this.getDataByField(19);
    return data;
  };
  // Field 20 | Wind Direction | degrees (0=N, 90=E, 180=S, 270=W, 0=calm) | range: 0–360 | missing: 999
  epw.windDirection = function() {
    data = this.getDataByField(20);
    return data;
  };
  // Field 21 | Wind Speed | m/s | range: 0–40 | missing: 999
  epw.windSpeed = function() {
    data = this.getDataByField(21);
    return data;
  };
  // Field 22 | Total Sky Cover | tenths (0=clear, 10=overcast) | range: 0–10 | missing: 99
  epw.totalSkyCover = function() {
    data = this.getDataByField(22);
    return data;
  };
  // Field 23 | Opaque Sky Cover | tenths (0=clear, 10=overcast) | range: 0–10 | missing: 99
  epw.opaqueSkyCover = function() {
    data = this.getDataByField(23);
    return data;
  };
  // Field 24 | Visibility | km | range: 0–9999 | missing: 9999
  epw.visibility = function() {
    data = this.getDataByField(24);
    return data;
  };
  // Field 25 | Ceiling Height | m | range: 0–9999 | missing: 99999
  epw.ceilingHeight = function() {
    data = this.getDataByField(25);
    return data;
  };
  // Field 26 | Present Weather Observation | 0=station obs, 9=missing | missing: 9
  epw.presentWeatherObservation = function() {
    data = this.getDataByField(26);
    return data;
  };
  // Field 27 | Present Weather Codes | 9-digit code (see EPW IDD Appendix B) | missing: 999999999
  epw.presentWeatherCodes = function() {
    data = this.getDataByField(27);
    return data;
  };
  // Field 28 | Precipitable Water | mm | range: 0–999 | missing: 999
  epw.precipitableWater = function() {
    data = this.getDataByField(28);
    return data;
  };
  // Field 29 | Aerosol Optical Depth | dimensionless (broadband turbidity) | range: 0–0.999 | missing: 0.999
  epw.aerosolOpticalDepth = function() {
    data = this.getDataByField(29);
    return data;
  };
  // Field 30 | Snow Depth | cm | range: 0–150 | missing: 999
  epw.snowDepth = function() {
    data = this.getDataByField(30);
    return data;
  };
  // Field 31 | Days Since Last Snowfall | days | range: 0–88 | missing: 99
  epw.daysSinceLastSnowfall = function() {
    data = this.getDataByField(31);
    return data;
  };
  // Field 32 | Albedo | dimensionless ratio (reflected / incident solar) | range: 0.1–1.0 | missing: 999
  epw.albedo = function() {
    data = this.getDataByField(32);
    return data;
  };
  // Field 33 | Liquid Precipitation Depth | mm | range: 0–999 | missing: 999
  epw.liquidPrecipitationDepth = function() {
    data = this.getDataByField(33);
    return data;
  };
  // Field 34 | Liquid Precipitation Quantity (collection interval) | hr | range: 0–24 | missing: 99
  epw.liquidPrecipitationQuantity = function() {
    data = this.getDataByField(34);
    return data;
  };

  // Import location data from header row 0. Example:
  // LOCATION,Denver Centennial  Golden   Nr,CO,USA,TMY3,724666,39.74,-105.18,-7.0,1829.0
  epw._location         = epw_raw[0];
  epw.stationLocation   = epw_raw[0][1]; // Station name | string
  epw.state             = epw_raw[0][2]; // State/province code | string
  epw.country           = epw_raw[0][3]; // Country code | string
  epw.source            = epw_raw[0][4]; // Data source (e.g. TMY3, IWEC) | string
  epw.stationID         = epw_raw[0][5]; // WMO station number | string
  epw.latitude          = epw_raw[0][6]; // Latitude | decimal degrees (+N, -S)
  epw.longitude         = epw_raw[0][7]; // Longitude | decimal degrees (+E, -W)
  epw.timeZone          = epw_raw[0][8]; // Time zone offset from UTC | hours (+E, -W)
  epw.elevation         = epw_raw[0][9]; // Site elevation above sea level | m

  //More header methods can go here

  //DATA PERIOD
  epw.dataPeriod        = epw_raw[7];

  //WEATHER DATA
  //remove header and parse weather data into weatherData object
  epw_raw.splice(0,8);
  epw.weatherData = epw_raw;
  epw.computePsychrometrics();

  return epw;
};