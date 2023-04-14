# Testing `geometry_preview.html` outside of the OS App

Run `openstudio generate_sample_data.rb` to generate the ThreeJS representation for the exampleModel.

Open `geometry_preview.html` in your browser. (or start an http server, via `npx serve` or `python -m http.server 8000`)

Open the developer console (F12)

Type this:

```
var depdata =  fetch("./sample_data.json").then((data) => {
    return data.json();
});

depdata.then(function(data) {
  console.log(data)
  init(data);
  animate();
  initDatGui();
});
```
