set(tests
	mdi_add
	mdi_sub
)

foreach(name ${tests})
	add_executable(test_${name} ${name}.c)
	target_link_libraries(test_${name} mdi bigint)
endforeach()
