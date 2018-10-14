target_sources(ThreadPoolDemo 
			PUBLIC 
				#${CMAKE_CURRENT_LIST_DIR}/TaskQueue.cpp
				#${CMAKE_CURRENT_LIST_DIR}/TaskQueue.h
				${CMAKE_CURRENT_LIST_DIR}/LF_TaskBase.h)
message("${CMAKE_CURRENT_LIST_DIR}")
