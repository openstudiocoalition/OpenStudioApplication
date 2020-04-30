<h1>Measure Code Examples</h1>
This page provides example Ruby code for doing various common tasks. It is intended as a supplement to the full [OpenStudio SDK documentation](https://s3.amazonaws.com/openstudio-sdk-documentation/index.html). The examples are not complete Measures for the sake of clarity.

## Unit Conversion
OpenStudio has a built-in unit conversion method:

```ruby
OpenStudio.convert()
```

This method will convert a value from the first unit to the second unit, as specified by two strings. The conversion can be from IP to SI, SI to IP, IP to another IP unit, or SI to another SI unit. Any to-from units that are valid conversions will result in an initialized Optional::Double (see [this] for an explanation of optional doubles).

```ruby
# Convert the value 212 from Fahrenheit to Celsius
val_in_celsius = OpenStudio.convert(val_in_fahrenheit,"F","C")

# Check that the conversion worked, then get the value
if val_in_celsius.is_initialized
  val_in_celsius = val_in_celsius.get
end
```

```ruby
# Or, if you know the unit conversion is valid, here's the short version
val_in_celsius = OpenStudio.convert(val_in_fahrenheit,"F","C").get
```

You can use the `*` (multiply), `/` (divide), and `^` (exponent) operators in the unit strings to combine basic units like `"ft"` and `"min"` into compound units like `"ft^3/min"`.

### Available units
<table class="table table-striped">
  <tr>
    <th>Unit Type</th>
    <th>IP</th>
    <th>SI</th>
  </tr>
  <tr>
    <td>Temperature</td>
    <td>"F"</td>
    <td>"C"</td>
  </tr>
  <tr>
    <td>DeltaTemperature</td>
    <td>"R"</td>
    <td>"K"</td>
  </tr>
  <tr>
    <td>Length</td>
    <td>"in", "ft", "mi"</td>
    <td>"m"</td>
  </tr>
  <tr>
    <td>People</td>
    <td>"person"</td>
    <td>"person"</td>
  </tr>
  <tr>
    <td>Time</td>
    <td>"s", "min", "h", "hr", "day"</td>
    <td>"s", "min", "h", "hr", "day"</td>
  </tr>
  <tr>
    <td>Mass</td>
    <td>"lb_m", "ton"</td>
    <td>"kg"</td>
  </tr>
  <tr>
    <td>Area</td>
    <td>"in^2", "ft^2"</td>
    <td>"m^2"</td>
  </tr>
  <tr>
    <td>Volume</td>
    <td>"in^3", "ft^3"</td>
    <td>"m^3", "L"</td>
  </tr>
  <tr>
    <td>Force</td>
    <td>"lb_f"</td>
    <td>"N"</td>
  </tr>
  <tr>
    <td>Power</td>
    <td>"ft\*lb_f/s"</td>
    <td>"W"</td>
  </tr>
  <tr>
    <td>Pressure</td>
    <td>"inH_{2}O", "ftH_{2}O", "inHg"</td>
    <td>"Pa"</td>
  </tr>
  <tr>
    <td>FlowRate</td>
    <td>"cfm", "ft^3/min", "gal/min", "gpd", "gal/day"</td>
    <td>"m^3/hr", "m^3/s", "L/hr", "L/s", "L/min", "L/day"</td>
  </tr>
  <tr>
    <td>U-Factor</td>
    <td>"Btu/ft^2\*h\*R"</td>
    <td>"W/m^2\*K"</td>
  </tr>
  <tr>
    <td>R-Value</td>
    <td>"ft^2\*h\*R/Btu"</td>
    <td>"m^2\*K/W"</td>
  </tr>
  <tr>
    <td>Conductivity</td>
    <td>"Btu/ft^2\*h\*R"</td>
    <td>"W/m^2\*K"</td>
  </tr>
  <tr>
    <td>Illuminance</td>
    <td>"fc"</td>
    <td>"lux"</td>
  </tr>
  <tr>
    <td>LuminousFlux</td>
    <td></td>
    <td>"lm"</td>
  </tr>
  <tr>
    <td>Angles</td>
    <td>"deg"</td>
    <td>"rad"</td>
  </tr>
  <tr>
    <td>Energy</td>
    <td>"Btu", "kBtu", "ft*lb_f"</td>
    <td>"J", "GJ"</td>
  </tr>
  <tr>
    <td>RotationalSpeed</td>
    <td>"rpm"</td>
    <td>"rpm"</td>
  </tr>
  <tr>
    <td>Cycles</td>
    <td>"cycle"</td>
    <td>"cycle"</td>
  </tr>
  <tr>
    <td>Frequency</td>
    <td>"Hz"</td>
    <td>"Hz"</td>
  </tr>
  <tr>
    <td>Velocity</td>
    <td>"in/s", "ft/s", "mph"</td>
    <td>"cm/s", "m/s"</td>
  </tr>
  <tr>
    <td>ElectricPotential</td>
    <td>"V"</td>
    <td>"V"</td>
  </tr>
  <tr>
    <td>ElectricResistance</td>
    <td>"ohm"</td>
    <td>"ohm"</td>
  </tr>
</table>
