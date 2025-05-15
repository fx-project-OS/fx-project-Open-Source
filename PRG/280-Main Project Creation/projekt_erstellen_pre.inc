<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_erstellen_pre.inc                                    //
// Version     : 24.1                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2024-03-14                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * PF 280: Create Main Project
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if($err)
{
	// ...Prefix
	$vPF='';
	if(isset($_POST['pprefix']))
		$vPF=$_POST['pprefix'];
	// ...Projektname (Name_Projekt)
	$vNP=$_POST['Name_Projekt'];
	// ...Projektnummer (Vorgangsnummer)
	$vVNS=$_POST['Vorgangsnummer'];

	// Hauptverantwortliche Personen
	// ...Ansprechp. Auftragg. (Ansprechpartner)
	$vAG=$_POST['Ansprechpartner'];
	// ...Projektmanager
	$vPM=$_POST['Projektmanager'];
	// ...Projektleiter
	$vPL=$_POST['Projektleiter'];
	// ...Stell. Projektleiter (Vertreter)
	$vSP=$_POST['vertreter'];

	// Termine (geplant)
	// ...Geplantes Startdatum (Soll_Beg_Dtm)
	$vSBD=$_POST['Soll_Beg_Dtm'];
	// ...Geplantes Enddatum (Soll_End_Dtm)
	$vSED=$_POST['Soll_End_Dtm'];

	// Budgetrahmen (Soll Nett)
	// ...Int. Personenbudget (budgetrahmen_pers_int)
	$vBRPI=$_POST['budgetrahmen_pers_int'];
	// ...Ext. Personenbudget (budgetrahmen_pers_ext)
	$vBRPE=$_POST['budgetrahmen_pers_ext'];
	// ...Int. Materialbudget (budgetrahmen_mat_int)
	$vBRMI=$_POST['budgetrahmen_mat_int'];
	// ...Ext. Materialbudget (budgetrahmen_mat_ext)
	$vBRME=$maskenfeldwerte['budgetrahmen_mat_ext'];
}
else
{
	// ...Get most used customer + contact person
	$vKD=0;
	$vAG=0;
	$sql="SELECT COUNT(ansprechpartner) AS anz, ansprechpartner FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." GROUP BY ansprechpartner ORDER BY anz DESC";
	$tmp=db_values($sql);
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
	{
		$vAG=(int)$tmp['ansprechpartner'];
		$vKD=(int)db_value("SELECT elter FROM personen WHERE personen_id=".$vAG." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	}

	// ...Get most used project manager
	$sql="SELECT projektmanager, COUNT(projekt_id) AS anz FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projektmanager>0 GROUP BY projektmanager ORDER BY anz DESC";
	$tmp=db_values($sql);
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
		$vPM=(int)$tmp['projektmanager'];
	else
	{
		$sql="SELECT personen_id FROM ma_art_faehigkeiten WHERE mafaehigkeit_id=-2 AND maart_id=1 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY dauer DESC, zeitstempel";
		$vPM=(int)db_value($sql);
	}

	// ...Get most used project leader
	$sql="SELECT projektleiter, COUNT(projekt_id) AS anz FROM projekte WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projektleiter>0 GROUP BY projektleiter ORDER BY anz DESC";
	$tmp=db_values($sql);
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
		$vPL=(int)$tmp['projektleiter'];
	else
	{
		$sql="SELECT personen_id FROM ma_art_faehigkeiten WHERE mafaehigkeit_id=1 AND maart_id=1 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY dauer DESC, zeitstempel";
		$vPL=(int)db_value($sql);
	}

	// Prefix (pprefix)
	$vPF=get_prefix($GLOBALS['fxpglobals']['client'], $vKD);

	// Projektname (Name_Projekt)
	$vNP='*'.meldung(564);

	// Projektnummer (Vorgangsnummer)
	$max_VN=(int)db_value("SELECT MAX(vorgangsnummer) FROM projekte WHERE projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	if($max_VN)
		$vVNS=$max_VN+1;
	else if(strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['vorgang0'], 0, 1)) == 'n')
		$vVNS=1;
	else
		$vVNS=0;
	$vVNS=min(9999, max(0,(int)$vVNS));

	// Hauptverantwortliche Personen
	// ...Stell. Projektleiter (Vertreter)
	$vSP=0;

	// Termine (geplant)
	// Geplantes Startdatum (Soll_Beg_Dtm)=Aktueller Tag
	$vSBD=$GLOBALS['datetime'];
	// Geplantes Enddatum (Soll_End_Dtm)=Monatsende
	$ma=1;
	if((int)substr((string)$vSBD,6,2) > 15)
		$ma=2;
	$vSED=fx_date('YmdHis', fx_mktime(12, 0, 0, (int)substr((string)$vSBD,4,2)+$ma, 0, (int)substr((string)$vSBD,0,4)));

	// Budgetrahmen (Soll Nett)
	// ...Int. Personenbudget (budgetrahmen_pers_int)
	$vBRPI=0.00;
	// ...Ext. Personenbudget (budgetrahmen_pers_ext)
	$vBRPE=0.00;
	// ...Int. Materialbudget (budgetrahmen_mat_int)
	$vBRMI=0.00;
	// ...Ext. Materialbudget (budgetrahmen_mat_ext)
	$vBRME=0.00;
}

// Maskenfelder
$prefix_wildcard='';
if(isset($GLOBALS['fxpglobals']['settings']['vnprefix']) && ($GLOBALS['fxpglobals']['settings']['vnprefix'] !== NULL))
	$prefix_wildcard=trim((string)$GLOBALS['fxpglobals']['settings']['vnprefix']);
if(strlen((string)$prefix_wildcard))
{
	// Feldlängen ermitteln
	$sql="SELECT anzeigegroesse2 FROM maskenfelder WHERE nummer=12 AND eingabefeld=349 AND anzeigeart=8";
	$fwp=abs(db_value($sql));
	$sql="SELECT anzeigegroesse1 FROM maskenfelder WHERE nummer=12 AND eingabefeld=46 AND anzeigeart=6";
	$fwn=abs(db_value($sql));

	// ...Prefix (pprefix) + Projektname (Name_Projekt)
	$field_pprefix=fieldText('pprefix',$vPF,$fwp,64);
	$field_Name_Projekt=fieldText('Name_Projekt',$vNP,$fwn,255);

	$Name_Projekt_lit=get_text(1900,'10').':<div style="height:8px;"></div>'.get_text(46,'15');
	$maskenfeldwerte['name_projekt']='~'.$field_pprefix.'<div style="height:2px;"></div>'.$field_Name_Projekt;
}
else
{
	// ...Projektname (Name_Projekt)
	$maskenfeldwerte['name_projekt']=$vNP;
}

// ...Projektnummer (Vorgangsnummer)
$maskenfeldwerte['vorgangsnummer']=$vVNS;

// Hauptverantwortliche Personen
// ...Ansprechp. Auftragg. (Ansprechpartner)
$maskenfeldwerte['ansprechpartner']=$vAG;
// ...Projektmanager
$maskenfeldwerte['projektmanager']=$vPM;
// ...Projektleiter
$maskenfeldwerte['projektleiter']=$vPL;
// ...Stell. Projektleiter (Vertreter)
$maskenfeldwerte['vertreter']=$vSP;

// Termine (geplant)
// ...Geplantes Startdatum (Soll_Beg_Dtm)
if(strlen((string)$vSBD))
	$maskenfeldwerte['soll_beg_dtm']=$vSBD;
// ...Geplantes Enddatum (Soll_End_Dtm)
if(strlen((string)$vSED))
	$maskenfeldwerte['soll_end_dtm']=$vSED;

// Budgetrahmen (Soll Nett)
// ...Int. Personenbudget (budgetrahmen_pers_int)
$maskenfeldwerte['budgetrahmen_pers_int']=$vBRPI;
// ...Ext. Personenbudget (budgetrahmen_pers_ext)
$maskenfeldwerte['budgetrahmen_pers_ext']=$vBRPE;
// ...Int. Materialbudget (budgetrahmen_mat_int)
$maskenfeldwerte['budgetrahmen_mat_int']=$vBRMI;
// ...Ext. Materialbudget (budgetrahmen_mat_ext)
$maskenfeldwerte['budgetrahmen_mat_ext']=$vBRME;
?>