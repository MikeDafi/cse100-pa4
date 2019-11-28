subdir('ActorNode')
subdir('ActorGraph')

pathfinder_exe = executable('pathfinder.cpp.executable', 
    sources: ['pathfinder.cpp'],
    dependencies : [actorgraph_dep,node_dep],
    install : true)

linkpredictor_exe = executable('linkpredictor.cpp.executable', 
    sources: ['linkpredictor.cpp'],
    dependencies : [actorgraph_dep,node_dep],
    install : true)

movietraveler_exe = executable('movietraveler.cpp.executable', 
    sources: ['movietraveler.cpp'],
    dependencies : [actorgraph_dep,node_dep],
    install : true)