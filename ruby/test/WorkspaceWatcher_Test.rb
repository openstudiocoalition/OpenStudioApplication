########################################################################################################################
#  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
#  See also https://openstudiocoalition.org/about/software_license/
########################################################################################################################

require 'openstudio'
require 'openstudio_modeleditor'

require 'minitest/autorun'

module OpenStudio

  class TestWorkspaceWatcher < WorkspaceWatcher

    attr_accessor :addedObjectHandle, :removedObjectHandle

    def initialize(workspace)
      @addedObjectHandle = nil
      @removedObjectHandle = nil
      super
    end

    def clearState
      super
      @addedObjectHandle = nil
      @removedObjectHandle = nil
      puts "clearState"
    end

    def onChangeWorkspace
      super
      puts "onChangeWorkspace"
    end

    def onBecomeDirty
      super
      puts "onBecomeDirty"
    end

    def onBecomeClean
      super
      puts "onBecomeClean"
    end

    def onObjectAdd(addedObject)
      super
      @addedObjectHandle = addedObject.handle
      puts "onObjectAdd"
    end

    def onObjectRemove(removedObject)
      super
      @removedObjectHandle = removedObject.handle
      puts "removedObject"
    end

  end

end


class WorkspaceWatcher_Test < MiniTest::Unit::TestCase

  def setup
    OpenStudio::Modeleditor::Application::instance().application(false)
  end

  # def teardown
  # end

  def test_WorkspaceWatcher

    workspace = OpenStudio::Workspace.new
    watcher = OpenStudio::TestWorkspaceWatcher.new(workspace)

    assert((not watcher.dirty))
    assert((not watcher.addedObjectHandle))
    assert((not watcher.removedObjectHandle))

    idfObject = OpenStudio::IdfObject.new("OS_Space".to_IddObjectType)
    addedObject = workspace.addObject(idfObject)
    assert((not addedObject.empty?))
    addedObjectHandle = addedObject.get.handle

    assert_equal(1, workspace.numObjects)
    assert(watcher.dirty)
    assert(watcher.objectAdded)
    assert(watcher.enabled)
    OpenStudio::Modeleditor::Application::instance.processEvents
    assert(watcher.addedObjectHandle)
    assert((not watcher.removedObjectHandle))
    assert_equal(addedObjectHandle.to_s, watcher.addedObjectHandle.to_s)

    watcher.clearState

    assert((not watcher.dirty))
    assert((not watcher.addedObjectHandle))
    assert((not watcher.removedObjectHandle))

    addedObject.get.remove

    assert_equal(0, workspace.numObjects)
    assert(watcher.dirty)
    assert((not watcher.addedObjectHandle))
    assert(watcher.removedObjectHandle)
    assert_equal(addedObjectHandle.to_s, watcher.removedObjectHandle.to_s)

  end

end


