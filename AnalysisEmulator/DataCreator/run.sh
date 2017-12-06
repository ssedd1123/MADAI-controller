madai_pca_decompose $1
madai_train_emulator $1
madai_generate_trace $1 $1.csv
madai_gnuplot_scatterplot_matrix $1.csv $1.pdf $1/parameter_priors.dat 50
madai_analyze_trace $1 ../$1.csv > $1.ana
