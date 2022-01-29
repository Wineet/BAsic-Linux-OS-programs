 Problem: Consider Multiple threads are Exectuing various different Task in continues loop.
		at certain point each thread come across a common function e.g. common_fun(arg)
		every thread comes with his own pointer which hold address of a memory location

		at some point one of thread crashes due to segmentation Fault, by core dump analysis
		you got to know segmantation fault occuring in "common_fun". Now we need to find out
		due to which thread this Fault occured.

Note: "common_fun" is a re-entrent and thread safe function 
