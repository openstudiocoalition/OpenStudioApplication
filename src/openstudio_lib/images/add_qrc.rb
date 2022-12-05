lines = []
File.open("openstudio.qrc", 'r').each_line do |line|
  if /@2x.png/.match(line)
    #puts "Removing: #{line}"
  elsif /.png/.match(line)
    lines << line
    new_line = line.gsub(".png", "@2x.png")
    #puts "Adding: #{new_line}"
    lines << new_line
  else
    lines << line
  end
end

File.open("openstudio.qrc", 'w') do |f|
  lines.each do |line|
    f.puts line
  end
end