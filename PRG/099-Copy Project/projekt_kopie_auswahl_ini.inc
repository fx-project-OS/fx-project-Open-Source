<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_kopie_auswahl_ini.inc                                //
// Version     : 21.2                                                         //
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
 * @version 21.2
 */

setForm(true);

// Variablen
// ...Quelle (Projekt)
$src_id=0;
if($GLOBALS['gproject'])	// Benutzer kommt vom Projektpopupmenü
	$src_id=$GLOBALS['gproject'];
else if(($_GET['uid'] == 'newtr') && $GLOBALS['fxpglobals']['project'])
	$src_id=$GLOBALS['fxpglobals']['project'];
else if($_POST['Projekt_ID'])
	$src_id=(int)$_POST['Projekt_ID'];
// ...Ziel (Projekt)
$dst_id=0;
if($GLOBALS['gproject'] || (($_GET['uid'] == 'newtr') && $GLOBALS['fxpglobals']['project']))
	$dst_id=(int)db_value("SELECT elter FROM projekte WHERE projekt_id=".$src_id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
else if($_POST['Elter'])
	$dst_id=(int)$_POST['Elter'];
// ...Teilprojekte anzeigen
$inf='';
if($_GET['tr'] || $_POST['Unterprojekte'])
	$inf=1;
// ...Ressourcen kopieren
$res='';
if($_POST['Ress_uebern'])
	$res=1;
// ...Zielwerte übernehmen
$dval='';
if($_POST['zielwerte_uebern'])
	$dval=1;
//echo('<b>$src_id:</b> '.$src_id.', <b>$dst_id:</b> '.$dst_id.', <b>$inf:</b> '.$inf.', <b>$res:</b> '.$res.'<br />, <b>$dval:</b> '.$dval.'<br />');

// Datenermitteln
$src_dat=array();
if($src_id)
{
	$sql  = "SELECT p.vorgangsnummer, p.ansprechpartner, p.projektmanager, p.projektleiter, p.vertreter, p.kategorie_id, z.soll_beginn, z.soll_ende";
	$sql .= " FROM projekte p, zeitdaten z";
	$sql .= " WHERE z.projekt_id=".$src_id." AND z.zeitart=102 AND p.projekt_id=".$src_id." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$src_dat=db_values($sql);

	$met=get_projekt_methode($src_id);
	$src_dat['methode']=$met['methode'];
	$src_dat['zeitpunkt']=$met['zeitpunkt'];
	$src_dat['tage']=$met['tage'];
//fxDebug($src_dat, $sql, 0);
}

$dst_dat=array();
if($dst_id)
{
	$sql  = "SELECT p.vorgangsnummer, p.ansprechpartner, p.projektmanager, p.projektleiter, p.vertreter, p.kategorie_id, z.soll_beginn, z.soll_ende";
	$sql .= " FROM projekte p, zeitdaten z";
	$sql .= " WHERE z.projekt_id=".$dst_id." AND z.zeitart=102 AND p.projekt_id=".$dst_id." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$dst_dat=db_values($sql);

	$met=get_projekt_methode($dst_id);
	$dst_dat['methode']=$met['methode'];
	$dst_dat['zeitpunkt']=$met['zeitpunkt'];
	$dst_dat['tage']=$met['tage'];
//fxDebug($dst_dat, $sql);
}

// Im Feld "Quelle (Vorgang)" muss ein Eintrag ausgewählt werden!
if(!$src_id && ($_POST['Button_Select'] || $_POST['Button_Submit'] || $_POST['Button_Multi'] || $_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck']))
{
	error_msg(789, array('Projekt_ID'), 'quelle');
	$err=true;
}

// Im Feld "Auftraggeber (Kunde)" muss ein Eintrag ausgewählt werden!
if(!$_POST['Ansprechpartner'] && ($_POST['Button_Submit'] || $_POST['Button_Multi'] || $_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck']))
{
	error_msg(789, false, 'Ansprechpartner');
	$err=true;
}

// Im Feld "Projektleiter" muss ein Eintrag ausgewählt werden!
if(!$_POST['Projektleiter'] && ($_POST['Button_Submit'] || $_POST['Button_Multi'] || $_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck']))
{
	error_msg(789, false, 'Projektleiter');
	$err=true;
}

// Im Feld "Ermittlungsmethode" muss ein Eintrag ausgewählt werden!
if(!$_POST['methode'] && ($_POST['Button_Submit'] || $_POST['Button_Multi'] || $_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck']))
{
	error_msg(789, false, 'methode');
	$err=true;
}

// Bitte geben Sie in das Feld "Berichtszeitpunkt" eine gültige, positive Zahl ein!
$tage=(int)$_POST['tage'];
if(($_POST['zeitpunkt'] == 1792) && ($tage < 1) && ($_POST['Button_Submit'] || $_POST['Button_Multi'] || $_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck']))
{
	error_msg(1524, array('tage'), 'zeitpunkt');
	$err=true;
}

// Datumsvalidierung
$sbd='';
$sed='';
if(($_POST['Button_Submit'] || $_POST['Button_Multi'] || $_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck']))
{
	$sbd=validate_entry($_POST['Soll_Beg_Dtm'], $verrb, 'datum');
	if($verrb)
	{
		error_msg($verrb, false, 'Soll_Beg_Dtm');
		$err=true;
	}
	$sed=validate_entry($_POST['Soll_End_Dtm'], $verre, 'datum');
	if($verre)
	{
		error_msg($verre, false, 'Soll_End_Dtm');
		$err=true;
	}
	// Es muss entweder das Start- oder das Enddatum, aber niemals beide angegeben werden!
	if((!$sbd && !$sed) || ($sbd && $sed))
	{
		error_msg(1836, array('Soll_Beg_Dtm', 'Soll_End_Dtm'));
		$err=true;
	}
}
$tad=0;
if(!$err && ($_POST['Button_Submit'] || $_POST['Button_Multi'] || $_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck']))
{
	if($sbd)
		$tad=round(time_diff($src_dat['soll_beginn'], $sbd, 'D'));
	else
		$tad=round(time_diff($src_dat['soll_ende'], $sed, 'D'));
}
//echo('<b>$tad:</b> '.$tad.' Tag(e)<br />');

if(is_array($_POST['Name_Projekt']))
{
	reset($_POST['Name_Projekt']);
	$mc	= key($_POST['Name_Projekt']);
}
else
	$mc=false;

// Checkbox-Validierung
$chkch=false;
if($_POST['Button_Multi'] && is_array($GLOBALS['_cppj']['data']) && ($mc !== false))
{
	$z=0;
	foreach($GLOBALS['_cppj']['data'] as $id => $d)
	{
		if(($d['md'] != 1) && ($d['md'] != 3) && !$_POST['Checkbox'][$mc][$z])
		{
			if($GLOBALS['_cppj']['data'][$id]['md'])
			{
				$GLOBALS['_cppj']['data'][$id]['md']=0;
				$chkch=true;
			}
			$nz=0;
			foreach($GLOBALS['_cppj']['data'] as $cid => $cd)
			{
				if(($nz >= $z) && $_POST['Checkbox'][$mc][$nz] && ($cd['elt'] == $id))
				{
					$GLOBALS['_cppj']['data'][$cid]['md']=0;
					unset($_POST['Checkbox'][$mc][$nz]);
					$chkch=true;
				}

				$nz++;
			}
		}
		else if(($d['md'] == 0) && $_POST['Checkbox'][$mc][$z])
		{
			$GLOBALS['_cppj']['data'][$id]['md']=2;
			$chkch=true;
		}

		$z++;
	}
}

if($_POST['Button_Multi'])
{
	if(!is_array($_POST['Checkbox']))
	{
		error_msg(1646);	// Es wurde keine notwendige Checkbox markiert!
		$err=true;
	}
	else if($chkch)
	{
		error_msg(1838);	// Einige Checkboxen wurden verändert! Bitte bestätigen Sie die Änderungen.
		$err=true;
	}
}

// Projektnamen-Validierung
if(is_array($GLOBALS['_cppj']['data']) && ($mc !== false))
{
	$z=0;
	foreach($GLOBALS['_cppj']['data'] as $id => $d)
	{
		$val_zeilennummer=$z;

		$np=trim($_POST['Name_Projekt'][$mc][$z]);
		if(!strlen($np) && ($d['md'] == 2))
		{
			error_msg(749, false, 'Name_Projekt');	// Das Feld $var1 darf nicht leer sein. Bitte geben Sie einen Text ein!
			$err=true;
		}
		else if(($d['md'] == 0) || ($d['md'] == 2))
			$GLOBALS['_cppj']['data'][$id]['npn']=$np;

		$z++;
	}
}

$osrc_id=(int)$GLOBALS['_cppj']['info']['sid'];

if($_POST['Button_Timeadjust1'])
	$timeadjust=1;
else if($_POST['Button_Timeadjust2'])
	$timeadjust=2;
else
	$timeadjust=0;
//echo('<b>$timeadjust:</b> '.$timeadjust.'<br />');

// Maske "Zusatzangaben" anzeigen
$pk_param=false;
if((!$err && $_POST['Button_Select']) || $_POST['Button_Submit'] || $_POST['Button_Multi'] || $_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck'])
	$pk_param=true;

// Maske "Vorschau" anzeigen
$pk_vorschau=false;
if((!$err && $_POST['Button_Submit']) || $_POST['Button_Multi'] || $_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck'])
	$pk_vorschau=true;

// DB
if($_POST['Button_Timeadjust1'] || $_POST['Button_Timeadjust2'] || $_POST['Button_Zurueck'])
	$err=true;
if($err || !$_POST['Button_Multi'])
	$db_update=false;
?>