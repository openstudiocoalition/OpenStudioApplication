Dir.glob("*.png").each do |f|
  next if /@2x.png/.match(f)

  md = /(.*).png/.match(f)
  if md

    command = "\"C:\\Program Files\\ImageMagick-7.1.0-Q16-HDRI\\magick\" convert #{md[1]}.png -adaptive-resize 78x84 #{md[1]}_tab@2x.png"

    command = "\"C:\\Program Files\\ImageMagick-7.1.0-Q16-HDRI\\magick\" convert #{md[1]}.png -filter Triangle -define filter:support=2 -resize 200% -unsharp 0.25x0.25+8+0.065 -dither None -posterize 136 -quality 82 -define jpeg:fancy-upsampling=off -define png:compression-filter=5 -define png:compression-level=9 -define png:compression-strategy=1 -define png:exclude-chunk=all -interlace none -colorspace sRGB -strip #{md[1]}@2x.png"
    puts command
    puts `#{command}`
  end
end