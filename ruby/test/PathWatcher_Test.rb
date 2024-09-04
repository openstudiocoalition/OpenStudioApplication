########################################################################################################################
#  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
#  See also https://openstudiocoalition.org/about/software_license/
########################################################################################################################

require 'openstudio'
require 'openstudio_modeleditor'

require 'minitest/autorun'

class TestPathWatcher < OpenStudio::Modeleditor::PathWatcher

  attr_accessor :changed

  def initialize(path)
    super
    @changed = false
  end

  def onPathChanged
	#puts 'onPathChanged'
	#STDOUT.flush
    @changed = true
  end

  def onPathRemoved
	#puts 'onPathRemoved'
	#STDOUT.flush
    @changed = true
  end

end

class PathWatcher_Test < MiniTest::Unit::TestCase

  def setup
    OpenStudio::Modeleditor::Application::instance().application(false)
  end

  # def teardown
  # end

  def test_watchFile
    p = OpenStudio::Path.new("./path_watcher_file")
    if OpenStudio::exists(p)
      OpenStudio::remove(p)
    end
    assert((not OpenStudio::exists(p)))

    File.open(p.to_s, 'w') do |f|
      f << "test 1"
	  begin
	    f.fsync
	  rescue
	    f.flush
	  end
    end
    assert(OpenStudio::exists(p))

    watcher = TestPathWatcher.new(p)
    assert((not watcher.changed))

    # write to the file
    File.open(p.to_s, 'w') do |f|
      f << "test 2"
	  begin
	    f.fsync
	  rescue
	    f.flush
	  end
    end

    # calls processEvents
    OpenStudio::System::msleep(10000)
    OpenStudio::Modeleditor::Application::instance().processEvents(10000)

    assert(watcher.changed)
    watcher.changed = false
    assert((not watcher.changed))

    OpenStudio::remove(p)
    assert((not OpenStudio::exists(p)))

    # calls processEvents
    OpenStudio::System::msleep(10000)
    OpenStudio::Modeleditor::Application::instance().processEvents(10000)

    assert(watcher.changed)
  end

  def test_watchDir
    p = OpenStudio::Path.new("./")
    assert(OpenStudio::exists(p))

    # try existent dir
    assert_raises(RuntimeError){TestPathWatcher.new(OpenStudio::Path.new(p))}

    # try non-existent dir
    assert_raises(RuntimeError){TestPathWatcher.new(OpenStudio::Path.new("./I do not exist/"))}
  end

end


