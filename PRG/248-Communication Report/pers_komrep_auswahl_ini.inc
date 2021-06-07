<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : pers_komrep_auswahl_ini.inc                                  //
// Version     : 21.1                                                         //
// Begin       : 2021-01-15                                                   //
// Last Change : 2021-01-15                                                   //
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
 * PF 248: Communication Report
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(fxIsArray($_POST) && isset($_POST['sf248_set']))
{
	$tpta=array(FXP_PT_CLIENT, FXP_PT_CLIENT_CP, FXP_PT_CUSTOMER, FXP_PT_CUSTOMER_CP, FXP_PT_PARTNER, FXP_PT_PARTNER_CP, FXP_PT_EMPLOYEE, FXP_PT_CONTRACTOR, FXP_PT_CONTACT, FXP_PT_CONTACT_CP);
	foreach($tpta as $tptai)
		$_POST['sf'.$_POST['sf248_set'].'_personentyp_'.$tptai]='c';
	$_POST['sf'.$_POST['sf248_set'].'_sortierreihe']='t';
}

$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=30 AND mandanten_id=0 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY tabwert";
$pt_arr=db_values($sql,'*');

// Rechte auf Personentypen ermitteln, auf dessen Transaktion der Benutzer Zugriff hat...
$ptr=personentyp_rechte(true);
// ...und welche im Filter aktiviert sind
if(fxIsArray($ptr) && fxIsArray($GLOBALS['fxpglobals']['filter']))
{
	$fptr=array();
	$fset=false;
	foreach($GLOBALS['fxpglobals']['filter'] as $feld => $wert)
	{
		if((substr($feld, 0, 12) == 'personentyp_') && (!$GLOBALS['fxpglobals']['setfilter'] || $GLOBALS['fxpglobals']['filter']['fperson_inaktiv'] || $wert))
			$fptr[(int)substr($feld, 12)] = true;
	}
	if(sizeof($fptr) < 10)
	{
		foreach($ptr as $apt => $dmy)
		{
			if(!isset($fptr[$apt]))
			{
				$fset=true;
				unset($ptr[$apt]);
			}
		}
	}

	// Bei Einschränkungen Filtericon zeigen
	if($fset)
		$maske493_flt=2565;
}

// Personenrechte
$pr_csv=get_persons();
if(!$pr_csv || ($pr_csv == '0'))
	$pr_csv=$GLOBALS['fxpglobals']['person'];
//fxDebug($pr_csv, '$pr_csv');

// Sprung aus Kaufwahrscheinlichkeit?
if(fxIsArray($_GET) && isset($_GET['sname']))
{
	$_POST['Suchtext']=$_GET['sname'];
	$_GET['blid']=1;
}

// Reportmaske einblenden?
if($_GET['blid'] == 1)
	$_POST['Button_Anzeigen']=true;
// Rücksprung
if(isset($_GET['back']))
	$_POST['Button_Anzeigen']=true;
$show_report=false;
if(!$err && $_POST['Button_Anzeigen'])
	$show_report=true;
?>