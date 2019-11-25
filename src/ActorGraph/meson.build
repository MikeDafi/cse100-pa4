inc = include_directories('./')
ActorGraphLibrary = library('ActionGraphLibrary', sources: ['ActorGraph.cpp', 'ActorGraph.hpp'],dependencies: [node_dep])

actorgraph_dep = declare_dependency(include_directories: inc,
  link_with: ActorGraphLibrary)