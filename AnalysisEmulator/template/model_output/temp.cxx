void temp()
{
	for(int i = 0; i < 49; ++i)
	{
		stringstream ss1, ss2;
		ss1 << "run" << setw(4) << setfill('0') << i;
		ss2 << "run" << setw(4) << setfill('0') << i + 1;
		system(("cp e120/" + ss2.str() + "/parameters.dat e120_sn112_spectra/" + ss1.str() + "/parameters.dat").c_str()); 
	}
}
