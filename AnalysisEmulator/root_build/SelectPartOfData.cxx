void SelectPartOfData()
{
	RunMaster controller("/mnt/analysis/hira/tsangc/new_distribution/e120", "e120_bugs_free/");
	std::vector<std::string> par_names, var_names;
	controller.GetParName(par_names);
	controller.GetVarName(var_names);

	std::vector<std::string> list_var(var_names.begin() + 6, var_names.begin() + 15);
	list_var.insert(list_var.end(), var_names.begin() + 22 , var_names.end());
	//list_var.insert(list_var.end(), var_names.begin() + 20, var_names.end() - 2);

	DataCreator creator(par_names, list_var);
	
	for(int i = controller.GetStartNo(); i <= controller.GetEndNo(); ++i)
	{
		std::vector<double> var, var_error;
		std::vector<double> par(controller.GetParFromRun(i));
		std::vector<Data> all_var(controller.GetVarFromRun(i));
		for(auto&& names : list_var)
		{
			auto it = std::find(var_names.begin(), var_names.end(), names);
			var.push_back(all_var[it - var_names.begin()].val);
			var_error.push_back(all_var[it - var_names.begin()].err);
		}

		creator.PushBack(par, var, var_error);
	}
	creator.GetController();
}
