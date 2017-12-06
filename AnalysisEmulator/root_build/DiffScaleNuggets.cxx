void DiffScaleNuggets(int t_no = 40)
{
	RunMaster controller("/mnt/analysis/hira/tsangc/new_distribution/e120", "e120/");
	// configurations to be looped through
	std::vector<double> nuggets;//{/*0.001*/pow(10, -0.42)};
	std::vector<double> scales;//{/*0.01*/1};
	for(double val = 0.001; val < 0.5; val +=0.1) nuggets.push_back(val);
	for(double val = 0.001; val < 2 ;val +=0.2) scales.push_back(val);

	for(auto&& val_nuggets : nuggets)
		for(auto&& val_scales : scales)
		{
			std::cout << " Working on scales = " << val_scales << " nuggets = " << val_nuggets << endl;
			controller.SetScales(val_scales);
			controller.SetNuggets(val_nuggets);
			controller.GenerateTrace();
		}
}
