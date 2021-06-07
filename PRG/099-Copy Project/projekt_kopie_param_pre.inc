<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_kopie_param_pre.inc                                  //
// Version     : 21.1                                                         //
// Begin       : 2020-12-10                                                   //
// Last Change : 2020-12-10                                                   //
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
 * 99: Copy Project
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$def_1=array(-1 => '['.meldung(2778,true).']');
// Default 2: -1=[Wert aus Quellvorgang kopieren], 0=-leer-
$def_2=$def_1;
$def_2[0]=meldung(794,true);

// Alle Felder zuordnen
$fa=array('*Ansprechpartner', '*Projektmanager', '*Projektleiter', '*Vertreter', '*Kategorie_ID', 'methode', 'zeitpunkt', 'tage');
foreach($fa as $fld)
{
	$pers=false;
	if(substr($fld,0,1) == '*')
	{
		$pers=true;
		$fld=substr($fld,1);
	}
	$lfld=strtolower($fld);
	if($_POST['Button_Select'])
	{
		if($dval && fxIsArray($dst_dat))
			$id=(int)$dst_dat[$lfld];
		else
			$id=-1;
	}
	else
		$id=(int)$_POST[$fld];

	if($pers)
	{
		$mva=array();
		if(fxIsArray($src_dat))
		{
			$sql="SELECT DISTINCT(".$lfld.") as pid FROM projekte WHERE vorgangsnummer LIKE '".$src_dat['vorgangsnummer']."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql, 0);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$mva[$t['pid']]=true;
			}
		}

		if(sizeof($mva))
		{
			$mvalues='';
			$fid=-1;
			foreach($mva as $pers => $dmy)
			{
				if(strlen($mvalues))
					$mvalues .= ',';
				$mvalues .= $pers;
				if($fid == -1)
					$fid=$pers;
			}
			$GLOBALS[$fld.'_arg']=array('mvalues'=>$mvalues);

			if($_POST['Button_Select'] && !fxIsArray($dst_dat))
			{
				if($dval && (sizeof($mva) == 1))
					$id=$fid;
				else
					$id=-1;
			}
		}
	}
	else
	{
		$mva=array();
		if(fxIsArray($src_dat))
		{
			$sql="SELECT DISTINCT(".$lfld.") as pid FROM projektfortschritt WHERE projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$src_dat['vorgangsnummer']."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'].") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql, 0);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
					$mva[$t['pid']]=true;
			}
		}

		if(sizeof($mva))
		{
			$fid=-1;
			foreach($mva as $sid => $dmy)
			{
				if($fid == -1)
				{
					$fid=$sid;
					break;
				}
			}
			if($_POST['Button_Select'] && !fxIsArray($dst_dat))
			{
				if($dval && (sizeof($mva) == 1))
					$id=$fid;
				else
					$id=-1;
			}
		}
	}

	$_POST[$fld]=$id;
	$maskenfeldwerte[$lfld]=$id;
}

// Auftraggeber (Kunde)
$Ansprechpartner_ds=0;
$Ansprechpartner_ds_array=$def_1;

// Projektmanager (Projektmanager)
$Projektmanager_ds_array=$def_2;

// Projektleiter (Projektleiter)
$Projektleiter_ds=0;
$Projektleiter_ds_array=$def_1;

// stell. Projektleiter (Vertreter)
$Vertreter_ds_array=$def_2;

// Geplantes Startdatum (Soll_Beg_Dtm) + Geplantes Enddatum (Soll_End_Dtm)
$sbd=$src_dat['soll_beginn'];
$sed='';
if(!$_POST['Button_Select'])
{
	if(isset($_POST['Soll_Beg_Dtm']))
		$sbd=validate_entry($_POST['Soll_Beg_Dtm'],$derr,'datum');
	else
		$sbd=$src_dat['soll_beginn'];
	if(isset($_POST['Soll_End_Dtm']))
		$sed=validate_entry($_POST['Soll_End_Dtm'],$derr,'datum');
	else
		$sed='';
}
$maskenfeldwerte['soll_beg_dtm']=$sbd;
$maskenfeldwerte['soll_end_dtm']=$sed;

// Projektkategorien (Kategorie_ID)
$Kategorie_ID_ds_array=$def_2;

// Ermittlungsmethode (methode)
$methode_ds=0;
$methode_ds_array=$def_1;

// Berichtszeitpunkt (zeitpunkt, tage)
$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=1660 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") ORDER BY position";
$tmp=db_values($sql,'*');
if(fxIsArray($tmp))
{
//function fieldRadio($fieldname, $value, $value_array, $direction='h', $maxcol=0, $dis=false, $nva=false, $tooltip_array=array(), $jsfunc='', $form='')
	$rba=array(-1=>$def_1[-1]);
	foreach($tmp as $t)
	{
		if($t['wertetabid'] == 1792)	// Alle [x] Tage...
			$txt=str_replace('[x]', fieldText('tage',$_POST['tage'],38,5), $t['tabwert']);
		else
			$txt=$t['tabwert'];
		$rba[$t['wertetabid']]=$txt;
	}
	$mfz=fieldRadio('zeitpunkt',$_POST['zeitpunkt'],$rba,'v');
}
else
	$mfz='STANDARD';
$maskenfeldwerte['zeitpunkt']='~'.$mfz;
?>