<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : db_tab_liste_pre.inc                                         //
// Version     : 21.1                                                         //
// Begin       : 2021-02-12                                                   //
// Last Change : 2021-04-14                                                   //
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
 * PF 140: Database Tables
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$txt_key = get_text(155, '20');	// Primärschlüssel
$txt_idx = get_text(156, '20');	// Indiziert
$txt_nnl = get_text(157, '20');	// NichtNull
$txt_cst = get_text(158, '20');	// Gruppiert

$txt_db = explode(';', meldung(15));	// Generierte Tabelle;System Tabelle;Daten Tabelle;Interne Tabelle;Individuelle Tabelle

// Globale Variablen für SQL-Definition
$crt1 = '';
$crt2 = '';
$crt4 = '';
$idx1 = '';
$idx2 = '';
$idx4 = '';
$repnl = array("\r\n"=>'<br>', "\r"=>'<br>', "\n"=>'<br>', "\t"=>'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;');

// Spaltenüberschriften nicht anzeigen, da selbst generiert wird für jede Tabelle
maskClearLC();
$maske328_nch=true;
$mc=$GLOBALS['_maskcounter'];

// Tabellen
$tbsql=$IDinTabselect;
$op=stripos($tbsql,'ORDER BY');
if($op)
	$tbsql=trim(substr($tbsql,0,$op));
if($IDinTab)
	$tbsql .= " AND id_tabellen=".(int)$IDinTab;
else if($TabelleArt)
	$tbsql .= " AND tabelleart=".(int)$TabelleArt;
if(!$IDinTab)
	$tbsql .= " ORDER BY tabname";
$tbarr=db_values($tbsql, '*');
//fxDebug($tbarr, $tbsql);

// Alle Tabellen durchgehen
$ln=0;
if(fxIsArray($tbarr))
{
	$ico_gpk=$GLOBALS['gfxpath'].'key_g_15x15.png';
	$ico_gix=$GLOBALS['gfxpath'].'lst_g_15x15.png';
	$ico_gnn=$GLOBALS['gfxpath'].'nnl_g_15x15.png';
	$ico_gcl=$GLOBALS['gfxpath'].'cls_g_15x15.png';

	$ico_bpk=$GLOBALS['gfxpath'].'key_b_15x15.png';
	$ico_bix=$GLOBALS['gfxpath'].'lst_b_15x15.png';
	$ico_bnn=$GLOBALS['gfxpath'].'nnl_b_15x15.png';
	$ico_bcl=$GLOBALS['gfxpath'].'cls_b_15x15.png';

	// Spaltenüberschriften
	$head=array(
		'primaerschluessel'	=> '^class_rhd|~<img src="'.$ico_gpk.'" style="margin-top:2px;" tooltip="'.$txt_key.'">',
		'position'			=> '^class_rhd|~'.get_text(148, '5'),
		'feldname'			=> '^class_rhd|~'.get_text(180, '10'),
		'feldtyp'			=> '^class_rhd|~'.get_text(181, '10'),
		'laenge'			=> '^class_rhd|~'.get_text(182, '10'),
		'nichtnull'			=> '^class_rhd|~<img src="'.$ico_gnn.'" style="margin-top:2px;" tooltip="'.$txt_nnl.'">',
		'indiziert'			=> '^class_rhd|~<img src="'.$ico_gix.'" style="margin-top:2px;" tooltip="'.$txt_idx.'">',
		'doppeltewerte'		=> '^class_rhd|~<img src="'.$ico_gcl.'" style="margin-top:2px;" tooltip="'.$txt_cst.'">',
		'beschreibung'		=> '^class_rhd|~'.get_text(38, '15')
	);
//fxDebug($head,'$head');

	// Feldbeschreibungen
	$fda=array();
	$ts=$tbsql;
	$op=stripos($ts,'ORDER');
	if($op)
		$ts=trim(substr($ts,0,$op));
	$fp=stripos($ts,'FROM');
	if($fp)
		$ts=trim(substr($ts,$fp));
	$sql="SELECT f.feldname, x.text_kurz FROM tab_felder t, felder f, texte x WHERE t.idintabellen IN (SELECT id_tabellen ".$ts.") AND f.id=t.idinfelder AND f.mandanten_id IS NOT NULL AND x.id_referenz=t.idinfelder AND x.id_ref_art=3 AND x.id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
			$fda[strtolower($t['feldname'])]=$t['text_kurz'];
	}
//fxDebug($fda,'$fda');

	$colspan=sizeof($head);
	foreach($tbarr as $tb)
	{
		$tbname=strtolower($tb['tabname']);
		$tbid=(int)$tb['id_tabellen'];
		$tbart=max(0,(int)$tb['tabelleart']);	// Bei Formulargenerator ist art<0

		// Get table info
		if(!fxIsArray($GLOBALS['_dbta']) || !fxIsArray($GLOBALS['_dbta'][$tbname]))
		{
			dbStructure($tbname);
//fxDebug($GLOBALS['_dbta'][$tbname],$tbname);
		}

		if(fxIsArray($GLOBALS['_dbta'][$tbname]))
		{

			// Leerzeile zwischen den einzelnen Tabellen
			if($ln)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]=array();
				foreach($head as $field => $dummy)
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln][$field]='~|';
				$ln++;
			}

			// Headline
			$bc='79aec7';
			$fc='ffffff';
			$txt='<div style="float:right;color:#444444;"><i>['.$txt_db[$tbart].']</i>&nbsp;&nbsp;&nbsp;</div><div class="s5b" style="color:#'.$fc.';text-shadow:2px 2px 2px #000000;">&nbsp;'.strtoupper($tbname).'</div>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['primaerschluessel']='^col_'.$bc.'" nowrap colspan="'.$colspan.'" style="padding:8px 2px;border:0;border-top-left-radius:12px;border-top-right-radius:12px;"><!--|~'.$txt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['beschreibung']='~-->'.$txt;
			$ln++;

			// Spaltenüberschriften
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]=$head;
			$ln++;

			// Cluster
			$cla=array();

			// Primärschlüssel
			$pkn='';
			$pkc=false;
			$pka=array();
			if(fxIsArray($GLOBALS['_dbta'][$tbname]['primarykey']))
			{
				$pkn=$GLOBALS['_dbta'][$tbname]['primarykey']['name'];
				$pkc=$GLOBALS['_dbta'][$tbname]['primarykey']['clusered'];
				$ca=explode(',', $GLOBALS['_dbta'][$tbname]['primarykey']['columns']);
				foreach($ca as $c)
				{
					$pka[$c]=true;
					if($pkc)
						$cla[$c]=$pkn;
				}
			}
//fxDebug($pka,'$pka');

			// Indexe
			$ixa=array('carray'=>array());
			if(fxIsArray($GLOBALS['_dbta'][$tbname]['indexes']))
			{
				foreach($GLOBALS['_dbta'][$tbname]['indexes'] as $ixn => $a)
				{
					$ixa[$ixn]=array('clustred'=>$a['clustered'], 'columns'=>$a['columns'], 'carray'=>array());
					$ca=explode(',', $a['columns']);
					foreach($ca as $c)
					{
						if(!isset($ixa['carray'][$c]))
							$ixa['carray'][$c]=$ixn;
						else
							$ixa['carray'][$c] .= ', '.$ixn;
						$ixa[$ixn]['carray'][$c]=true;
						if($a['clustered'])
						{
							if(!isset($cla[$c]))
								$cla[$c]=$ixn;
							else
								$cla[$c] .= ', '.$ixn;
						}
					}
				}
			}
//fxDebug($ixa,'$ixa');
//fxDebug($cla,'$cla');

			// Tabellenspalten
			$cp=1;
			foreach($GLOBALS['_dbta'][$tbname]['columns'] as $tbspalte => $ca)
			{
				// (PK)
				if(fxIsArray($pka) && isset($pka[$tbspalte]))
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['primaerschluessel']='~<img src="'.$ico_bpk.'" style="margin-top:2px;cursor:help;" tooltip="'.$txt_key.': '.$pkn.'">';
					$fc='blue';
				}
				else
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['primaerschluessel']='~<div style="height:20px;"></div>';
					$fc='black';
				}
				// Pos.
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['position']='~<font class="grey">'.$cp.'</font>';
				// Feldname
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['feldname']='~<b class="'.$fc.'">'.$tbspalte.'</b>';
				// Feldtyp
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['feldtyp']='~<font class="darkgrey">'.$GLOBALS['_ftypes'][$ca['itype']].'</font>';
				if($_POST['Komplett'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['feldtyp'] .= '&nbsp;&nbsp;<i class="s2 lightgrey">('.dbColumnType($ca['ftype']).')</i>';
				// Länge
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['laenge']='~<font class="grey">'.substr($ca['ftype'],1).'</font>';
				// (NN)
				if($ca['not_null'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nichtnull']='~<img src="'.$ico_gnn.'" style="margin-top:2px;cursor:help;" tooltip="'.$txt_nnl.'">';
				// (IX)
				if(fxIsArray($ixa) && fxIsArray($ixa['carray']) && isset($ixa['carray'][$tbspalte]))
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['indiziert']='~<img src="'.$ico_gix.'" style="margin-top:2px;cursor:help;" tooltip="'.$txt_idx.': '.$ixa['carray'][$tbspalte].'">';
				// (CL)
				if(fxIsArray($cla) && isset($cla[$tbspalte]))
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['doppeltewerte']='~<img src="'.$ico_gcl.'" style="margin-top:2px;cursor:help;" tooltip="'.$txt_cst.': '.$cla[$tbspalte].'">';
				// Beschreibung
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['beschreibung']='~<font class="grey">'.$fda[$tbspalte].'</font>';

				$cp++;
				$ln++;
			}

			// SQL-Definition
			if($Komplett && $tb['tabelleart'])
			{
				// CREATE table with primary Key
				$pks=dbCreatePrimaryKeyDef($pkn, $pkc, $pka);
				$sql=fxHtmlEncode(dbCreateTableSQL($tbname, $GLOBALS['_dbta'][$tbname]['columns'], $pks, $pka));

				// CREATE index
				if(fxIsArray($GLOBALS['_dbta'][$tbname]['indexes']))
				{
					foreach($GLOBALS['_dbta'][$tbname]['indexes'] as $ixn => $a)
					{
						$la=array(3=>$ixn, $a['columns'], $a['clustered']);
						$isql=dbCreateIndexSQL($tbname,$la);
						if(strlen($isql))
							$sql .= $GLOBALS['nl_f'].$GLOBALS['nl_f'].fxHtmlEncode($isql);
					}
				}

				if(strlen($sql))
				{
					$sql='<span class="grey" style="padding:0;margin:0;">'.fxHtmlEncode($sql).'</span>';
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['primaerschluessel']	= '^class_rv'.($ln%2).'" colspan="'.$colspan.'" style="padding:12px;background:#ffffff;border-top:1px solid #'.$bc.';box-shadow:1px 1px 2px rgba(0,0,0,0.25) inset;"><!--|~'.$sql;
					$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['beschreibung']		= '~-->'.$sql;
					$ln++;
				}
			}

			// Ende
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['primaerschluessel']='^col_'.$bc.'" nowrap colspan="'.$colspan.'" style="padding:4px 24px;border:1px solid #e8e8e8;border-top:0;border-bottom-left-radius:8px;border-bottom-right-radius:8px;"><!--|~';
			$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['beschreibung']='~-->';
			$ln++;
		}
	}
}

if(!$ln)
	$GLOBALS['fxptdata']['lvalues'][$mc]=true;
?>