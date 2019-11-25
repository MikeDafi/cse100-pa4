inc = include_directories('./')


actorNodeLibrary = library('actorNodeLibrary', sources: ['ActorNode.hpp'],dependencies: [])

node_dep = declare_dependency(include_directories: inc,
  link_with: actorNodeLibrary)