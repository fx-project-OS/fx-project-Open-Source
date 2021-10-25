<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : fxp_db_pre.inc                                               //
// Version     : 21.2                                                         //
// Begin       : 2020-12-22                                                   //
// Last Change : 2020-12-22                                                   //
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
 * PF 250: Database Backup
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$index=true;

// Maskenzähler
$mc=$GLOBALS['_maskcounter'];

// NoWrap Felder
$Datum_nw			= true;
$version_nr_nw		= true;
$Dateiname_nw		= true;
$dateigroesse_nw	= true;

// DMS
if(!class_exists('fxpDoc'))
{
	$GLOBALS['__includer']['dms']=true;
	require('includer.inc');
}
if(!isset($GLOBALS['doc']))
	$GLOBALS['doc']=new fxpDoc();
$dms_path=$GLOBALS['doc']->cpath;

// Eindeutige ID
$dts=$GLOBALS['fxpglobals']['person'].'_'.time();

// Sollen Backups gelöscht werden?
$chks=array();
if($_POST['Button_Loeschen'])
{
	if(is_array($_POST['Checkbox'][$mc]))
	{
		$sc=0;
		foreach($_POST['Checkbox'][$mc] as $z => $chk)
		{
			$chks[$_POST['nv_text1'][$mc][$z]]=1;
			$did=(int)$GLOBALS['fxptdata']['backup']['lwidx'][$z]['did'];

			// Delete document (without logs, except if $lid is set (>0 one entry, <0 all antries))
			$success=$GLOBALS['doc']->deleteDoc($did, 0, -1);
			if($success)
			{
				$nr=146;	// $var1 wurde erfolgreich gelöscht.
				$sc++;
			}
			else
			{
				$nr=151;	// Fehler beim Löschen von $var1!
				$err=true;
			}
			error_msg($nr, false, '"<b>'.$_POST['nv_text1'][$mc][$z].'</b>"');
		}
	}
	else
		error_msg(1642);	// Es wurden keine Einträge angehakt.
}

// Backup-Option immer ganz oben
$dbs_date=substr($GLOBALS['datetime'],0,12);
$dbs['99999999999999']=array
(
	'id'	=> 0,
	'name'	=> '_fxp_mbackup'.$GLOBALS['fxpglobals']['client'].'_'.$dbs_date.'_'.$GLOBALS['fxpglobals']['dbparam']['versionscode_db'].'.sql',
	'size'	=> 0,
	'date'	=> $dbs_date.'00',
	'vers'	=> $GLOBALS['fxpglobals']['dbparam']['versionscode_db']
);

// Alle physikalisch vorhanden alten Sicherungen im DMS-Ordner ermitteln
$searchstring='_fxp_mbackup'.$GLOBALS['fxpglobals']['client'].'_';
$searchlen=strlen($searchstring);

$sql  = "SELECT d.dok_id, d.elter_art, d.elter, d.dok_art, d.id, d.dok_originalname, d.zeitstempel, v.dok_verlauf_id, v.datei_name, v.pfad, v.dateigroesse";
$sql .= " FROM d_dokumente d, d_dok_verlauf v";
$sql .= " WHERE v.dok_id=d.dok_id AND d.elter_art=2 AND v.aktiv_kz=1 AND d.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND v.mandanten_id=".$GLOBALS['fxpglobals']['client'];
$dms=db_values($sql, '*');
//fxDebug($dms,$sql, 0);

// Alte Datenbanksicherungen ermitteln
if(fxIsArray($dms))
{
	foreach($dms as $d)
	{
		if((substr($d['datei_name'], $searchlen-1, 1) == '_') && (substr($d['datei_name'], $searchlen+12, 1) == '_'))
		{
			$dbs_date=substr($d['datei_name'], $searchlen, 12).'00';
//echo('<b>datei_name</b>: '.$dbs_date.'<hr />');
		}
		else if((substr($d['dok_originalname'], $searchlen-1, 1) == '_') && (substr($d['dok_originalname'], $searchlen+12, 1) == '_'))
		{
			$dbs_date=substr($d['dok_originalname'], $searchlen, 12).'00';
//echo('<b>dok_originalname</b>: '.$dbs_date.'<hr />');
		}
		else
		{
			$dbs_date=$d['zeitstempel'];
//echo('<b>zeitstempel</b>: '.$dbs_date.'<hr />');
		}
		$dbs[$dbs_date]=array
		(
			'id'	=> $d['dok_id'],
			'name'	=> $d['datei_name'],
			'file'	=> $d['pfad'],
			'size'	=> $d['dateigroesse'],
			'elt'	=> $d['elter'],
			'art'	=> $d['elter_art'],
			'date'	=> $dbs_date,
			'vers'	=> substr($d['dok_originalname'], $searchlen+13, 9)
		);
	}

	// Datenbanksicherungen nach Datum sortieren, sodass aktuellstes Datum immer ganz oben
	if(fxIsArray($dbs))
		krsort($dbs);
}

// Den "Lösch"-Button und die Checkboxen benötigen wir nur, falls alte Sicherungen vorhanden
if(sizeof($dbs) == 1)
{
	$Checkbox_nv=true;
	$Button_Loeschen_nv=true;
}

// Liste anzeigen
if(fxIsArray($dbs))
{
//fxDebug($dbs, '$dbs');
	$z=0;
	$m=-1;

	// No right border in the following column
	fxf_maskColumnBorder(array('Checkbox', 'link'), 0, -1, $mc);

	foreach($dbs as $dd => $d)
	{
		// Backup der aktuellen Version
		if($dd == '99999999999999')
		{
			$m=0;

			$lpre='<font class="blue">';
			$lpst='</font>';

			if($GLOBALS['fxpglobals']['lang'] == 1)
				$tt='Backup - Aktuelle Datenbank in das DMS sichern';
			else
				$tt='Backup - Store actual database to DMS';

			$llnk=fieldLinkImage('link'.$z, 'icne', 'loader.php&url=fxp_db_backup.inc&fxpsid='.$GLOBALS['fxpglobals']['sesstoid'].'&mod=1&mid='.$GLOBALS['fxpglobals']['client'].'&pid='.$GLOBALS['fxpglobals']['person'].'&vers='.$d['vers'].'&date='.$d['date'].'&did=-1', $tt);
		}
		// Restore/Delete
		else
		{
			// Restore möglich, da Versionen identisch
			if($d['vers'] == $GLOBALS['fxpglobals']['dbparam']['versionscode_db'])
				$nm=1;
			else
				$nm=2;

			if($nm != $m)
			{
				$m=$nm;
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']		= '~|';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']			= '^class_transparent" style="border-top:1px solid #cccccc;|~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['version_nr']		= '^class_transparent" style="border-top:1px solid #cccccc;|~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['dateiname']		= '^class_transparent" style="border-top:1px solid #cccccc;|~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['dateigroesse']	= '^class_transparent" style="border-top:1px solid #cccccc;|~';
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']			= '~|';
				$z++;
			}

			$GLOBALS['fxptdata']['backup']['lwidx'][$z]['dnm']=$d['file'];
			$GLOBALS['fxptdata']['backup']['lwidx'][$z]['onm']=$d['name'];
			$GLOBALS['fxptdata']['backup']['lwidx'][$z]['did']=$d['id'];
			$GLOBALS['fxptdata']['backup']['lwidx'][$z]['elt']=$d['elt'];
			$GLOBALS['fxptdata']['backup']['lwidx'][$z]['art']=$d['art'];
			$GLOBALS['fxptdata']['backup']['lwidx'][$z]['usr']=true;

			// Restore möglich, da Versionen identisch
			if($m == 1)
			{
				$lpre='';
				$lpst='';

				if($GLOBALS['fxpglobals']['lang'] == 1)
					$tt='Restore - Diese Datenbanksicherung aus dem DMS widerherstellen';
				else
					$tt='Restore - Restore this database backup from the DMS';

				$llnk=fieldLinkImage('link'.$z, 'icmo', 'loader.php?url=fxp_db_backup.inc&fxpsid='.$GLOBALS['fxpglobals']['sesstoid'].'&mod=2&mid='.$GLOBALS['fxpglobals']['client'].'&pid='.$GLOBALS['fxpglobals']['person'].'&vers='.$d['vers'].'&date='.$d['date'].'&did='.$d['id'], $tt);
			}
			// Restore nicht mehr möglich, da Versionen unterschiedlich
			else
			{
				$lpre='<i class="grey">';
				$lpst='</i>';

				$llnk='';
			}
		}

		// Checkbox
		if(!$m)
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='~|';
		else
		{
			$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox']='^class_|';
			if($err)
				$GLOBALS['fxptdata']['lvalues'][$mc][$z]['checkbox'] .= $chks[$d['name']];
		}

		// Datum
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['datum']='~'.$lpre.get_entry($d['date'], 'datetime').$lpst;

		// VersionsNr
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['version_nr']='~'.$lpre.(int)substr($d['vers'], 0, 3).'.'.(int)substr($d['vers'], 3, 3).'.'.(int)substr($d['vers'], 6).$lpst;

		// Dateiname
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['dateiname']='~'.$lpre.$d['name'].$lpst;

		// Dateigröße
		if(!$d['size'])
			$mbsize=0.00;
		else if($d['size'] < 1049)
			$mbsize=0.01;
		else
			$mbsize=$d['size']/1048576;
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['dateigroesse']='~'.$lpre.get_entry($mbsize, 'dezimal').'&nbsp;MB'.$lpst;

		// Link
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['link']='~'.$llnk;

		// Hidden
		$GLOBALS['fxptdata']['lvalues'][$mc][$z]['nv_text1']=$d['name'];

		$z++;
	}

	// Last line with Top Border?
	if($z)
	{
		$fa=fxf_getMaskFields(497,false,array('Checkbox'=>0,'link'=>0));
		fxf_maskColumnBorder($fa, 1, -1, $mc, $z, true);
		$z++;
	}
}
?>