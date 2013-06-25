#!/usr/bin/perl -w    
use Switch;

$dir = $ARGV[0]; 
$file = $ARGV[1];

$nb_groupes = `grep groupes $ARGV[0]|wc -l|sed \'s/ //g\'`;
$nb_plages = `grep port $ARGV[0]|wc -l|sed \'s/ //g\'`;
chomp($nb_plages);
chomp($nb_groupes);

#les structures dans lesquelles on stocke les données
@groupes=();
@ports=();
@int_1=();
@int_2=();
@ipv_1=();
@ipv_2=();
@ttl_1=();
@ttl_2=();

#on compte le nombre de lignes, pour signaler l'erreur
$num_ligne = 0;


open(fFile, "< $file") or die "cannot open file $file";
while($line = <fFile>){
	$num_ligne++;
	#on ignore les lignes commentées ou vides 
	if ((substr ($line,0,1) ne "#") && ($line =~ /=/)) {
		@ligne=split("=", $line);
		#si l'argument est vide, alors le programme sort avec une erreur
		chomp($ligne[1]);
		if(!length($ligne[1])){
			printf "Erreur dans le fichier de conf à la ligne $num_ligne.\n";
			exit;
		}                                                               
		
		#on met les données dans les structures adéquates
		switch ($ligne[0]){
			case "int_1"	{push(@int_1,$ligne[1])}
			case "int_2"	{push(@int_2,$ligne[1])}
			case "ipv_1"	{push(@ipv_1,$ligne[1])}
			case "ipv_2"	{push(@ipv_2,$ligne[1])}
			case "ttl_1"	{push(@ttl_1,$ligne[1])}
			case "ttl_2"	{push(@ttl_2,$ligne[1])}
			case "groupes"	{push(@groupes,$ligne[1])}
			case "ports"	{push(@ports,$ligne[1])}
		}

	}
	
}
close(fFile);

#On construit la commande et on l'affiche
$commande = "$dir/reflector $int_1[0] $int_2[0] $ipv_1[0] $ipv_2[0] $ttl_1[0] $ttl_2[0]";
                                         
#on rajoute le nb de groupes et les groupes
$commande = $commande." ".@groupes;
foreach $groupe (@groupes){
	@couple = split (",", $groupe);
	$commande = $commande." $couple[0] $couple[1]";
}

#on rajoute le nb de ports et les ports
$commande = $commande." ".@ports;
foreach $port (@ports){
	@couple = split (",", $port);
	$commande = $commande." $couple[0] $couple[1]";
}
                                                   
$commande = $commande."\n";

system($commande);
