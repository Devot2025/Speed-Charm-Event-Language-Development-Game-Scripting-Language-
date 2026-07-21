name EVENT1{
	obj v1 = 2025 + 1;
	start{
		@ example code systemcall
		obj buf = "Hello World! " + v1;
		__write__(stdout, buf);
	}
}
