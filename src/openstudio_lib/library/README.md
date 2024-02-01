# Testing `geometry_preview.html` outside of the OS App

Run `openstudio generate_sample_data.rb` to generate the ThreeJS representation for the exampleModel.

Open `geometry_preview.html` in your browser. (or start an http server, via `npx serve` or `python -m http.server 8000`)

Open the developer console (F12)

Type this:

```
runFromFile("./sample_data.json", true)
```

The second parameter, the boolean, is whether you want to enable the Geometry Diagnostics UI or not
