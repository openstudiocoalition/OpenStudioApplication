# Icon sources

SVG sources for the Plant Loop Heat Pump icons. The app loads the PNGs in `images/` and `images/mini_icons/`,
not these files.

| SVG | PNGs |
|---|---|
| `<name>.svg` | `images/<name>.png` (100 px) and `images/<name>@2x.png` (200 px) |
| `<name>_mini.svg` | `images/mini_icons/<name>.png` (24 px) and `images/mini_icons/<name>@2x.png` (48 px) |

The air-to-water wrapper and its heating and cooling modes share `heatpump_airtowater`.

## Exporting

`export.html` draws one SVG at a given size on a transparent page. Screenshot it with headless Chrome, once per size:

```sh
chrome --headless=new --hide-scrollbars --allow-file-access-from-files --default-background-color=00000000 \
  --window-size=200,200 --screenshot=heatpump_airtowater@2x.png \
  "file:///<path to this folder>/export.html?f=heatpump_airtowater.svg&s=200"
```

The f(x) labels on the water-to-water (equation fit) icons use the system's Georgia font.
