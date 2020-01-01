def fixup(file, rpath_replacement)
  puts
  puts file
  `otool -L #{file}`.split("\n").each do |line|
    line.strip!
    if md = /(@rpath[^\s]*)/.match(line)
      line = md[1]
      new_line = line.gsub("@rpath", rpath_replacement)
      if md = /Qt.*\/(Qt.*?)$/.match(new_line)
      	new_line = new_line.gsub(md[0], md[1])
      end
      puts "#{line} -> #{new_line}"
     `install_name_tool -change #{line} #{new_line} #{file}`
    end
  end
end

dir = File.dirname(ARGV[0])
fixup(ARGV[0], "@loader_path")
fixup(File.join(dir, "QtConcurrent"), "@loader_path")
fixup(File.join(dir, "QtCore"), "@loader_path")
fixup(File.join(dir, "QtGui"), "@loader_path")
fixup(File.join(dir, "QtNetwork"), "@loader_path")
fixup(File.join(dir, "QtPrintSupport"), "@loader_path")
fixup(File.join(dir, "QtWidgets"), "@loader_path")
fixup(File.join(dir, "QtXml"), "@loader_path")
fixup(File.join(dir, "platforms/libqcocoa.dylib"), "@loader_path/..")