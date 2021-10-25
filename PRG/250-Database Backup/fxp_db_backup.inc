<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : fxp_db_backup.inc                                            //
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

// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

$TA=array(0=>timecheck());

$req_fn=fxf_fn_reqFilename('db_tools', 250);
if(strlen($req_fn))
	require($req_fn);

// Fehlermeldungs-Array
$fma=array
(
	1 => array
	(
		1 =>	'DMS-Klasse NICHT verfügbar',
				'FALSCHE Session ID',
				'FALSCHE Mandanten ID',
				'FALSCHE Personen ID',
				'FALSCHE Version',
				'KEIN Datum',
				'FALSCHER Modus',
				'KEINE ID'
	),
	7 => array
	(
		1 =>	'DMS-Class NOT available',
				'WRONG Session ID',
				'WRONG Client ID',
				'WRONG Person ID',
				'WRONG Version',
				'NO Date',
				'WRONG Mode',
				'NO ID'
	)
);
//fxDebug('_GET');

$bvalue='Reload';
$tdstyle=' nowrap';

// Sprache
$lng=$GLOBALS['fxpglobals']['lang'];

// Prüfen, ob Programm ausgeführt werden darf
$msg='';

// We need our class
if(!class_exists('fxpDoc'))
{
	$GLOBALS['__includer']['dms']=true;
	require('includer.inc');
}
// ...DMS-Klasse NICHT verfügbar
if(!class_exists('fxpDoc'))
	$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC001:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$fma[$lng][1].'</td></tr>';
// ...Falsche Session Storage ID
if(!isset($_GET['fxpsid']) || !strlen($_GET['fxpsid']) || ($_GET['fxpsid'] != $GLOBALS['fxpglobals']['sesstoid']))
	$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC002:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$fma[$lng][2].'</td></tr>';
// ...Falsche Mandanten ID
if(!isset($_GET['mid']) || !strlen($_GET['mid']) || ($_GET['mid'] != $GLOBALS['fxpglobals']['client']))
	$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC003:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$fma[$lng][3].'</td></tr>';
else
	$mid=(int)$_GET['mid'];
// ...Falsche Personen ID
if(!isset($_GET['pid']) || !strlen($_GET['pid']) || ($_GET['pid'] != $GLOBALS['fxpglobals']['person']))
	$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC004:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$fma[$lng][4].'</td></tr>';
else
	$pid=(int)$_GET['pid'];
// ...Falsche Version
if(!isset($_GET['vers']) || !strlen($_GET['vers']) || ($_GET['vers'] != $GLOBALS['fxpglobals']['dbparam']['versionscode_db']))
	$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC005:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$fma[$lng][5].'</td></tr>';
else
	$vers=$_GET['vers'];
// ...Kein Datum
if(!isset($_GET['date']) || !strlen($_GET['date']))
	$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC006:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$fma[$lng][6].'</td></tr>';
else
	$dat=$_GET['date'];
// ...Falscher Modus
if(!isset($_GET['mod']) || !strlen($_GET['mod']) || ((int)$_GET['mod'] < 1) || ((int)$_GET['mod'] > 2))
	$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC007:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$fma[$lng][7].'</td></tr>';
else
	$mod=(int)$_GET['mod'];
// ...Keine ID
if(!isset($_GET['did']) || !strlen($_GET['did']))
	$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC008:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$fma[$lng][8].'</td></tr>';
else
	$did=(int)$_GET['did'];

// Mandanten-Ordner anlegen, falls noch nicht vorhanden
if(!strlen($msg))
{
	$cfld=$GLOBALS['datpath'].$mid.'/';
	if(!is_dir($cfld))
	{
		if(!@mkdir($cfld, 0777))
		{
			if($lng == 1) 	// Deutsch
				$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC008:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>Mandanten-Ordner ['.$cfld.'] konnte nicht erzeugt werden oder ist schreibgesch&uuml;tzt</td></tr>';
			else
				$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC008:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>Client folder ['.$cfld.'] could not be created or is write protected</td></tr>';
		}
		else
		{
			@chmod($cfld, 0777);
			@chown($cfld, 'nobody');
		}
	}
}

// DMS
if(!isset($GLOBALS['doc']))
	$GLOBALS['doc']=new fxpDoc();

// Hauptroutine: Backup bzw. Restore vorbereiten
if(!strlen($msg))
{
	// Variablen vorbelegen
	$mtab		= array();
	$anz_tab	= 0;
	$anz_sql	= 0;

	// Backup
	if($mod == 1)
	{
		// Alle Mandantentabellen ermitteln
		$mtab=dbt_get_mtabellen($mid, true);
		if(fxIsArray($mtab))
		{
			foreach($mtab as $tabname => $m)
			{
				$anz_tab++;
				$anz_sql += $m['anz'];
			}
		}
		else
		{
			if($lng == 1) 	// Deutsch
				$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC009:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>Keine Eintr&auml;ge in den Mandanten-Tabellen gefunden!</td></tr>';
			else
				$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC009:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>No entries found in client tables!</td></tr>';
		}
	}

	// Restore
	if($mod == 2)
	{
		$dms_path=$GLOBALS['doc']->cpath;
		if($did > 0)
		{
			$sql="SELECT pfad FROM d_dok_verlauf WHERE dok_id=".$did." AND mandanten_id=".$mid." AND aktiv_kz=1";
			$fnm=db_value($sql);
			if(strlen($fnm))
				$dms_filename=$dms_path.$fnm;
			else
				$dms_filename=$dms_path.'unknown';
		}
		else
			$dms_filename='unknown';

		if(file_exists($dms_filename))
		{
			$be=0;
			$bf=file($dms_filename);
			if(fxIsArray($bf))
			{
//fxDebug($bf, $dms_filename);
				$tabname='';
				foreach($bf as $z => $s)
				{
					if(substr($s, 0, 1) === 'T')					// Tabelle
					{
						$tabname='';
						$fnl=strpos($s, '|', 1);
						if(!$fnl)
							continue;

						$s=substr($s, $fnl+1);
						$nsp=strpos($s, ' ');
						if($nsp)
						{
							$tabname=strtolower(trim(substr($s, 0, $nsp)));
							if(!$mtab[$tabname])
							{
								$anz_tab++;
								$mtab[$tabname]=array('anz'=>0, 'bed'=>'');
							}
						}
					}
					else if($tabname && substr($s, 0, 1) === 'I')	// Insert
					{
						$mtab[$tabname]['anz']++;
						$anz_sql++;
					}
					else
						$tabname='';
				}
			}
			else
				$be |= 1;													// Bit 0=1:		Datei leer!

			// Fehlerprüfung
			if(!$anz_tab)
				$be |= 2;													// Bit 1=2:		Keine Tabellen gefunden?
			if(!$anz_sql)
				$be |= 4;													// Bit 2=4:		Keine SQL gefunden?

			// Prüfung Titelzeile
			// fx-project: BACKUP 2-200707261950-004000000-4-TSFXP4N-5|~µ#XPVUCLEECHEGCLCCCGCHCHCICCEECLCDCKCDCLCGEECJEBCFCCCGECCGEBCDCECJCLECKJCA
			$tz=trim($bf[0]);
			if((substr($tz, 0, 22) !== '/'.'* fx-project: BACKUP ') || (substr($tz, -3) !== ' *'.'/'))
				$be |= 8;													// Bit 3=8:		Falsche Titelzeilenkennung
			else
			{
				$tp=strpos($tz, '|', 22);
				if(!$tp)
					$be |= 16;												// Bit 4=16:		Trennzeichen?
				else
				{
					$tz1=substr($tz, 22, $tp-22);
					$tz2=db_pw_codec_db(substr($tz, $tp+1, -3));
					if(md5($tz1) !== $tz2)
						$be |= 32;											// Bit 5=32:		Codierungsmanipulation?
					else
					{
						$tza=explode('-', $tz1);
						if(sizeof($tza) != 6)
							$be |= 64;										// Bit 6=64:		Argumentenanzahl?
						else
						{
							if(($tza[0] != $GLOBALS['fxpglobals']['client']) || ($tza[0] != $mid))
								$be |= 128;									// Bit 7=128:		Mandantennummer?
							if($tza[1] != substr($dat, 0, 12))
								$be |= 256;									// Bit 8=256:		Datum?
							if(($tza[2] != $GLOBALS['fxpglobals']['dbparam']['versionscode_db']) || ($tza[2] != $vers))
								$be |= 512;									// Bit 9=512:		Versionsnummer?
							if($GLOBALS['fxpglobals']['person'] && ($tza[4] != $GLOBALS['fxpglobals']['dbparam']['dbname']))
								$be |= 1024;								// Bit 10=1024:	Datenbank-Name
							if($GLOBALS['fxpglobals']['person'] && ($tza[5] != $GLOBALS['fxpglobals']['dbparam']['dbtype']))
								$be |= 2048;								// Bit 11=2048:	Datenbank-Typ
						}
					}
				}
			}

			if($be)
			{
				if($lng == 1) 	// Deutsch
					$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC010:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>Fehler <b>#'.$be.'</b> in Backup-Datei<br>['.$dms_filename.']</td></tr>';
				else
					$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC010:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>Error <b>#'.$be.'</b>in backup file<br>['.$dms_filename.']</td></tr>';
			}
		}
		else
		{
			if($lng == 1) 	// Deutsch
				$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC011:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>Backup-Datei ['.$dms_filename.'] nicht gefunden!</td></tr>';
			else
				$msg .= '<tr><td nowrap valign="top"><font class="bold red">EC011:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>Backup file ['.$dms_filename.'] not found!</td></tr>';
		}
	}
}

// Hauptroutine: Backup bzw. Restore ausführen
if(!strlen($msg))
{
/*
	$phptxt ='$anz_tab='.$anz_tab.';<br>';
	$phptxt .= '$anz_sql='.$anz_sql.';<br>';
	if(is_array($mtab))
	{
		foreach($mtab as $tabname => $m)
			$phptxt .= '$mtab[\''.$tabname.'\']=array(\'anz\'=>'.$m['anz'].', \'bed\'=>\''.$m['bed'].'\');<br>';
	}
	else
		$phptxt .= '$mtab=false;<br>';

	$msg='<tr><td nowrap>'.$phptxt.'</td></tr>';
*/

	$GLOBALS['dontAddClient']=true;

	// Dateiname für Temp
	$tmp_filename=$GLOBALS['datpath'].$mid.'/'.$dat.'-'.$pid.'_tmp.sql';

	$dbtype=$GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['type'];

	// Ersatz für Zeilenumbruch
	$nl_z	= '|<*>|';
	$nl_rep=array("\r\n"=>$nl_z, "\r"=>$nl_z, "\n"=>$nl_z);
	$ct_rep=array('CREATE TABLE '=>'', '[dbo].'=>'', '['=>'', ']'=>'');
	$incsuf=array('db', 'db_user', 'ini', 'ini_user', 'pre', 'pre_user');

	$fileline=0;

	// Backup
	if($mod == 1)
	{
		// Temporäres File erzeugen
		if(@file_exists($tmp_filename))
		{
			@chmod($tmp_filename, 0777);
			unlink($tmp_filename);
		}

		$fp=fopen($tmp_filename, 'w');
		if($fp)
		{
// /* fx-project: BACKUP 68-200510111217-002001001-4376-fxp-4|~µ#FRRWEIMCHVCHECJFCLBCGRCJUCLZCFOCJOEEXCJGEHMCFEEGVEGKCMAEFSCLLCIKCKRCKYCECEETCNPCMBCJPCJLCIKCMUEFHCHXDLIB */
			$head=$mid.'-'.substr($dat, 0, 12).'-'.$vers.'-'.$pid.'-'.$GLOBALS['fxpglobals']['dbparam']['dbname'].'-'.$GLOBALS['fxpglobals']['dbparam']['dbtype'];
			$md5c=db_pw_codec_db(md5($head));
			fwrite($fp, "/"."* fx-project: BACKUP ".$head."|".$md5c." *"."/".$nl_b);
			$fileline++;

			// Get all field types for CREATE
			dbt_get_fieldtypes();

			// Write field types (Memo, Float + Integer)
			$mfi=array('M'=>'memo', 'F'=>'float', 'N'=>'integer');
			foreach($mfi as $smd => $ft)
			{
				if(fxIsArray($GLOBALS['fxpglobals']['dbparam'][$ft]))
				{
					$fld='';
					foreach($GLOBALS['fxpglobals']['dbparam'][$ft] as $fname => $dmy)
					{
						if(strlen($fld))
							$fld .= ',';
						$fld .= $fname;
					}
					if(strlen($fld))
					{
						fwrite($fp, line_code($fileline, $smd, $fld));
						$fileline++;
					}
				}
			}

			// Spezialbehandlung von Maskengenerator-Tabellen
			$mgt=array();
			if(fxIsArray($mtab['tabellen']))
			{
				// CREATE + PRIMARY KEY + INDEX
				dbt_get_tabellen(substr($mtab['tabellen']['bed'], 11), '=2');
				$sql_all =dbt_get_sqldef($GLOBALS['fxpglobals']['dbparam']['dbtype'], 'dat_create');
				$sql_all .= dbt_get_sqldef($GLOBALS['fxpglobals']['dbparam']['dbtype'], 'dat_index');
//echo('<pre>'.fxHtmlEncode($sql_all).'</pre>');
				if(strlen($sql_all))
				{
					$cpos=strpos($sql_all, '/'.'* fx-project');
					$sql_array=''; unset($sql_array);
					if($cpos !== false)	// SQL einzeln abarbeiten
						$sql_array=explode('/'.'* fx-project', $sql_all);
					else
						$sql_array=explode($nl_b, $sql_all);
					$errcnt=0;
					foreach($sql_array as $sql)
					{
						$sql=trim($sql);
						if(!strlen($sql))
							continue;

						// SQL ermitteln
						$head='';
						if(substr($sql, 0, 1) == ':')
						{
							$ecpos=strpos($sql, '*'.'/');
							if($ecpos !== false)
							{
								$head=trim(substr($sql, 1, $ecpos-1));
								$sql=trim(substr($sql, $ecpos+2));
							}
							else
								$sql='/'.'*'.substr($sql, 1);
						}

						if($head && $sql)
						{
//fxDebug($sql, 'HEAD: '.$head);
							$smd=strtoupper(substr($head, 7, 1));
							// Nur Create Tables, Primary Keys und Index schreiben
							if(($smd == 'T') || ($smd == 'P') || ($smd == 'I'))
							{
								if($smd == 'T')
									$smd='C';
								else if($smd == 'I')
									$smd='X';
								$sql=strtr($sql, $nl_rep);

								fwrite($fp, line_code($fileline, $smd, $sql));
								$fileline++;

								if($smd == 'C')
								{
									$fnl=strpos($sql, '|');
									if($fnl)
									{
										$tn=trim(strtr(substr($sql, 0, $fnl), $ct_rep));
										$mgt[$tn]=true;
									}
								}
							}
						}
					}
				}
				// Dateien
				if(sizeof($mgt))
				{
					foreach($mgt as $tabname => $m)
					{
						foreach($incsuf as $suf)
						{
							$incfn='_'.strtolower($tabname).'_'.$suf.'.inc';
							$inc=fxLoad($incfn);
							if($inc)
							{
								fwrite($fp, line_code($fileline, 'F', $incfn."|".strtr($inc, $nl_rep)));
								$fileline++;
							}
						}
					}
				}
			}

			// INSERTS
			foreach($mtab as $tabname => $m)
			{
				$buffer='';

				$sql="SELECT * FROM ".$tabname." WHERE ".$m['bed']." ORDER BY zeitstempel";
				$rid=db_query($sql);
				if($rid)
				{
					$cols='';
					while($arr=db_get_array($rid))
					{
						// Spaltennamen in erste Zeile
						if(!strlen($cols))
						{
							foreach($arr as $cn => $val)
							{
								if(strlen($cols))
									$cols .= ',';

								$cols .= $cn;
							}

							fwrite($fp, line_code($fileline, 'T', $tabname." (".$cols.")"));
							$fileline++;
						}

						// Spalteninhalte
						$vals='';
						foreach($arr as $cn => $val)
						{
							if(strlen($vals))
								$vals .= ',';

							if(!strlen($val) || ($val === 'NULL'))
								$vals .= "NULL";
							else if(isset($GLOBALS['fxpglobals']['dbparam']['float'][$cn]))
								$vals .= get_double($val);
							else if(isset($GLOBALS['fxpglobals']['dbparam']['integer'][$cn]))
								$vals .= (int)$val;
							else
								$vals .= "'".convert_string($val, 'todb')."'";
						}

						$buffer .= line_code($fileline, 'I', "(".strtr($vals, $nl_rep).");");
						$fileline++;
					}
				}

				if(strlen($buffer))
					fwrite($fp, $buffer);
			}

			fclose($fp);
			@chmod($tmp_filename, 0777);

			// Neues DMS-Element anlegen
			$dms_filename='_fxp_mbackup'.$mid.'_'.substr($dat,0,12).'_'.$GLOBALS['fxpglobals']['dbparam']['versionscode_db'].'.sql';

			$sql="SELECT personen_id FROM personen WHERE personentyp=274 AND mandanten_id=".$mid;
			$pidmid=(int)db_value($sql);

			// Dokument-ID
			$seid='2_1769/'.$pidmid.'.0.0';	// 2_1769=Backups (elter_art=2, dok_art=1769), $pidmid=Mandant (elter)

			// Upload-Attribute
			$uarr=array(
				'error'		=> 0,
				'name'		=> $dms_filename,
				'size'		=> filesize($tmp_filename),
				'new_name'	=> $tmp_filename
			);

			// Dokument-Attribute
			$values=array(
				'beschreibung' => 'BACKUP DB V'.$GLOBALS['fxpglobals']['dbparam']['version_db']
			);

			// Dokument in das DMS hinzufügen
			$rc=$GLOBALS['doc']->uploadDocument($seid, $uarr, $values);	// Rückgabewert wird vernachlässigt

			// Temporäre Datei löschen
			if(@file_exists($tmp_filename))
				unlink($tmp_filename);
		}
		else
		{
			if($lng == 1) 	// Deutsch
				$msg .= '<tr><td valign="top"><font class="red">Fehler:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>Tempor&auml;re Datei ['.$tmp_filename.'] konnte nicht erzeugt werden!</td></tr>';
			else
				$msg .= '<tr><td valign="top"><font class="red">Error:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>Temporary file ['.$tmp_filename.'] could not be created!</td></tr>';
		}

		if(!strlen($msg))
		{
			if($lng == 1)
				$msg='<tr><td nowrap>Die Datenbank wurde erfolgreich gesichert.</td></tr>';
			else
				$msg='<tr><td nowrap>The database backup was successful.</td></tr>';
		}
	}

	// Restore
	if($mod == 2)
	{
		$dms_path=$GLOBALS['doc']->cpath;
		if($did > 0)
		{
			$sql="SELECT pfad FROM d_dok_verlauf WHERE dok_id=".$did." AND mandanten_id=".$mid." AND aktiv_kz=1";
			$fnm=db_value($sql);
			if(strlen($fnm))
				$dms_filename=$dms_path.$fnm;
			else
				$dms_filename=$dms_path.'unknown';
		}
		else
			$dms_filename='unknown';

		if(file_exists($dms_filename))
			$bf=file($dms_filename);
		else
			$bf=false;

		if(fxIsArray($bf))
		{
			// Rollback-Marke setzen
			db_rollback('start');

			// Maskengeneratortabellen ermitteln
			$sql="SELECT name_maske FROM masken WHERE mandanten_id=".$mid;
			$mga=db_values($sql, '*');

			// Alte Daten des Mandanten löschen
			$otab=dbt_get_mtabellen($mid, true);
			if(!strlen($msg) && fxIsArray($otab))
			{
				// Maskenfelder + Texte zuerst, da diese von anderen abhängig sind...
				$otf=array('maskenfelder', 'texte');
				foreach($otf as $tabname)
				{
					if(fxIsArray($otab[$tabname]))
					{
						$bed=trim($otab[$tabname]['bed']);
						if(strlen($bed))
						{
							$sql="DELETE FROM ".$tabname." WHERE ".$bed;
//echo('<b>DELETE (1):</b> '.$sql.'<hr>');
							$success=db_query($sql);
							// Fehler beim DELETE
							if(!$success)
							{
								$terr=$sql;
								$dber=db_err();
								if($dber)
									$terr .= ' {'.$dber.'}';
								if($lng == 1) 	// Deutsch
									$msg .= '<tr><td valign="top"><font class="red">SQL-Fehler:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
								else
									$msg .= '<tr><td valign="top"><font class="red">SQL Error:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
								break;
							}
						}
						unset($otab[$tabname]);
					}
				}
				// ..dann den Rest
				if(!strlen($msg) && sizeof($otab))
				{
					foreach($otab as $tabname => $m)
					{
						$bed=trim($m['bed']);
						if(strlen($bed))
						{
							$sql="DELETE FROM ".$tabname." WHERE ".$bed;
//echo('<b>DELETE (2):</b> '.$sql.'<hr>');
							$success=db_query($sql);
							// Fehler beim DELETE
							if(!$success)
							{
								$terr=$sql;
								$dber=db_err();
								if($dber)
									$terr .= ' {'.$dber.'}';
								if($lng == 1) 	// Deutsch
									$msg .= '<tr><td valign="top"><font class="red">SQL-Fehler:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
								else
									$msg .= '<tr><td valign="top"><font class="red">SQL Error:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
								break;
							}
						}
					}
				}
			}

			// Alte FILES merken + Maskengenerator-Tabellen dropen
			$files=array();
			if(!strlen($msg) && fxIsArray($mga))
			{
				foreach($mga as $m)
				{
					$tabname=strtolower(substr($m['name_maske'], 1));

					// Alte FILES
					foreach($incsuf as $suf)
					{
						$incfn='_'.$tabname.'_'.$suf.'.inc';
						$inc=fxLoad($incfn);
						if($inc)
							$files['old'][$incfn]=$inc;
					}

					$success=true;

					// Maskengenerator-Tabellen dropen, falls existiert
					if($success)
					{
						$mgex=db_check($tabname, 't');
						if($mgex)
						{
							$sql="DROP TABLE ".$tabname.$GLOBALS['fxpglobals']['dbparam']['dbcascade'];
//echo('<pre>'.fxHtmlEncode($sql).'</pre><hr>');
							$success=db_query($sql);
						}
					}

					// Fehler
					if(!$success)
					{
						$terr=$sql;
						$dber=db_err();
						if($dber)
							$terr .= ' {'.$dber.'}';
						if($lng == 1) 	// Deutsch
							$msg .= '<tr><td valign="top"><font class="red">SQL-Fehler:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
						else
							$msg .= '<tr><td valign="top"><font class="red">SQL Error:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
						break;
					}
				}
			}

			// Daten aus Backup-File einspielen
			if(!strlen($msg))
			{
				$tabname	= '';
				$into		= '';
				$identity	= '';

				foreach($bf as $z => $s)
				{
					// Headerzeile überspringen - wurde bereits vorher überprüft
					if(!$z)
						continue;

					$dber=false;

					// Inhalt decodieren
					$ds=line_decode($z, $s);

// Manipulierte Zeile
					if($ds == 'DE')
					{
						if($lng == 1) 	// Deutsch
							$msg .= '<tr><td valign="top"><font class="red">Manipulations-Fehler:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>L:'.($z+1).'</td></tr>';
						else
							$msg .= '<tr><td valign="top"><font class="red">Manipulation Error:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>L:'.($z+1).'</td></tr>';
					}

//M|anmerkung,anmerkungintern,anmerkung_1,anmerkung_2,argumente,auftragsziel,beschreibung,bestaetigung_anmerk...
//F|abschreibungssatz,aktwert,allg_zahlenfeld,anschaffungspreis,anschaffungspreis_ges,arbeitstage,aufwand_ist...
//N|abgerufen_von,abhaengigkeit,abrechnungsart,abrufen,abrufen_all,abrufen_proj,abrufen_sel,abschreibungsart...
					else if((substr($ds, 0, 2) === 'M|') || (substr($ds, 0, 2) === 'F|') || (substr($ds, 0, 2) === 'N|'))	// Spezialfelder
					{
						if(substr($ds, 0, 2) === 'M|')
							$sft='memo';
						else if(substr($ds, 0, 2) === 'F|')
							$sft='float';
						else
							$sft='integer';

						$sfa=explode(',', strtolower(substr($ds, 2)));
						if(is_array($sfa))
						{
							foreach($sfa as $sfn)
								$GLOBALS['fxpglobals']['dbparam'][$sft][trim($sfn)]=true;
						}
					}

//C|CREATE TABLE [dbo].[FG_HartmannMaske_66]|<*>|(|<*>|	[mandanten_id] [INT] NULL,|<*>|	[hartmannmaske_id] [INT] NOT NULL...
//P|ALTER TABLE [dbo].[FG_HartmannMaske_66] WITH NOCHECK ADD CONSTRAINT [PK_FG_HartmannMaske_66] PRIMARY KEY NONCLUSTERED...
//X|CREATE INDEX [idx_dat211_000910005] ON [dbo].[FG_HartmannMaske_66] ([hartmannmaske_id]) ON [PRIMARY];
					else if((substr($ds, 0, 2) === 'C|') || (substr($ds, 0, 2) === 'P|') || (substr($ds, 0, 2) === 'X|'))	// CREATE, PRIMARY KEY + INDEX
					{
//echo('$ds='.$ds.'<br>');
						// Zur Sicherheit beim CREATE TABLE vorher prüfen, ob Tabelle auch wirklich nicht vorhanden
						if(substr($ds, 0, 2) === 'C|')
						{
							$fnl=strpos($ds, '|', 2);
							if($fnl)
							{
								$tn=strtolower(trim(strtr(substr($ds, 2, $fnl-2), $ct_rep)));
								$dber=db_table($tn, true);
							}
						}
						if(!$dber)
						{
							$sql=str_replace($nl_z, $nl_b, trim(substr($ds, 2)));
							$dber=dbtSQL('32', $sql);
						}
						if($dber)
						{
							$terr=$sql.' {'.$dber.'}';
							if($lng == 1) 	// Deutsch
								$msg .= '<tr><td valign="top"><font class="red">SQL-Fehler:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
							else
								$msg .= '<tr><td valign="top"><font class="red">SQL Error:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
						}
					}

//F|_fg_hartmannmaske_66_db.inc|Fileinhalt...
					else if(substr($ds, 0, 2) === 'F|')				// FILE
					{
						$hpos=strpos($ds, '|', 2);
						if($hpos)
						{
							$incfn	= substr($ds, 2, $hpos-2);
							$inc	= str_replace($nl_z, $nl_b, trim(substr($ds, $hpos+1)));

							$files['new'][$incfn]=$inc;
						}
					}

//T|abteilung (mandanten_id,abt_id,zu_abteilung,abt_ebene,abt_name,abt_hperson,aenderungs_id,zeitstempel)
					else if(substr($ds, 0, 2) === 'T|')				// TABLE
					{
						$into		= trim(substr($ds, 2));
						$tabname	= strtolower(trim(substr($into, 0, strpos($into, ' '))));

						// IDENTITY_INSERT bei MS-SQL setzen, d.h. Auto-ID Felder können überschrieben werden
						$success=true;
						if(strlen($identity) && ($dbtype == 'sqlsrv'))
						{
							$sql="SET IDENTITY_INSERT ".$identity." OFF";
							$success=db_query($sql);
							if(!$success)
							{
								if($lng == 1) 	// Deutsch
									$msg .= '<tr><td valign="top"><font class="red">SQL-Fehler:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$sql.'</td></tr>';
								else
									$msg .= '<tr><td valign="top"><font class="red">SQL Error:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$sql.'</td></tr>';
							}
						}

						if($success)
						{
							$identity='';
							$identity_pos_autoid=strpos($into, 'autoid,');
							if($identity_pos_autoid)
								$identity=$tabname;

							if(strlen($identity) && ($dbtype == 'sqlsrv'))
							{
								$sql="SET IDENTITY_INSERT ".$identity." ON";
								$success=db_query($sql);
								if(!$success)
								{
									if($lng == 1) 	// Deutsch
										$msg .= '<tr><td valign="top"><font class="red">SQL-Fehler:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$sql.'</td></tr>';
									else
										$msg .= '<tr><td valign="top"><font class="red">SQL Error:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$sql.'</td></tr>';
								}
							}
						}
					}
//I|(66, 760000, 0, 'FI', 'Finanzabteilung', 3103, 3072, '20041121111356');
					else if(strlen($into) && substr($ds, 0, 2) === 'I|')	// Insert
					{
						$sql="INSERT INTO ".$into." VALUES ".str_replace($nl_z, $nl_b, trim(substr($ds, 2)));
//echo('<pre>'.fxHtmlEncode($sql).'</pre>');
						$dber=dbtSQL('32', $sql);
						if($dber)
						{
							$terr=$sql.' {'.$dber.'}';
							if($lng == 1) 	// Deutsch
								$msg .= '<tr><td valign="top"><font class="red">SQL-Fehler:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
							else
								$msg .= '<tr><td valign="top"><font class="red">SQL Error:</font>&nbsp;&nbsp;</td><td'.$tdstyle.'>'.$terr.'</td></tr>';
						}
					}
					else
					{
						$into		= '';
						$tabname	= '';
					}
				}

				if(strlen($identity) && ($dbtype == 'sqlsrv'))
				{
					$sql="SET IDENTITY_INSERT ".$identity." OFF";
					$success=db_query($sql);
				}
			}

			// Bei einem Fehler alle Änderungen rückgängig
			if(strlen($msg))
				db_rollback('rollback');
			else
				db_rollback('ok');
		}

		if(!strlen($msg))
		{
			// FILES löschen bzw. erstellen
			if(sizeof($files))
			{
				foreach($files as $ftyp => $f)
				{
					if(fxIsArray($f))
					{
						foreach($f as $incfn => $inc)
						{
							if($ftyp == 'old')			// Aktuelle Dateien löschen
							{
								if(@file_exists($incfn))
								{
									@chmod($incfn, 0777);
									unlink($incfn);
								}
							}
							else if($ftyp == 'new')		// Backup-Dateien wieder herstellen
							{
								$fp=fopen($incfn, 'w');
								if($fp)
								{
									fwrite($fp, $inc);
									fclose($fp);
									@chmod($incfn, 0777);
								}
							}
						}
					}
				}
			}

			// Mandantenmenüdateien löschen
			deleteClientMenuFiles(true, true);
		}

		if(!strlen($msg))
		{
			if($lng == 1)
				$msg='<tr><td nowrap>Die Datenbank wurde erfolgreich wiederhergestellt.</td></tr>';
			else
				$msg='<tr><td nowrap>The database restore was successful.</td></tr>';

			$bvalue='Logout';
		}
	}
}

echo('<div id="dcbpu" class="maskc" style="position:relative;padding:16px;white-space:normal;">');
echo('<table id="pop_ic_close_excl" width="100%" border="0" cellspacing="0" cellpadding="0">');
echo($msg);
echo('</table><br><br>');
echo('<table width="100%" border="0" cellspacing="0" cellpadding="0"><tr><td align="center">');
echo(fieldSubmit(strtolower($bvalue), $bvalue, 'sub', 100, false, false, false, '', '', '', '', true));
echo('</td></tr></table>');
$TA[1]=timecheck();
//	echo('<br><br>Timecheck: '.$TA[0].' - '.$TA[1].' -- <b>'.($TA[1]-$TA[0]).' sec</b>');
echo('</div>');


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $fileline - Mandatory parameter: ???PARAMETER???
 * @param various $smd      - Mandatory parameter: ???PARAMETER???
 * @param various $txt      - Mandatory parameter: ???PARAMETER???
 */
function line_code($fileline, $smd, $txt)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$txt	= trim($txt);
	$md5cs	= line_md5($fileline, $smd, $txt);

	return($smd.'^'.$md5cs.'|'.$txt.$GLOBALS['nl_b']);
}

/**
 * ???FUNCTION???
 *
 * @param various $fileline - Mandatory parameter: ???PARAMETER???
 * @param various $line     - Mandatory parameter: ???PARAMETER???
 */
function line_decode($fileline, $line)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rs='DE';
	if(substr($line, 1, 1) == '^')	// codierte Zeile
	{
		$fnl=strpos($line, '|', 5);
		if($fnl)
		{
			$smd	= substr($line, 0, 1);
			$txt	= trim(substr($line, $fnl+1));
			$md5ln	= substr($line, 2, $fnl-2);
			$md5cs	= line_md5($fileline, $smd, $txt);

			// Rückgabe nur, falls Checksumme übereinstimmt
			if(strcmp($md5ln, $md5cs) == 0)
				$rs=$smd.'|'.$txt;
		}
	}
	return($rs);
}

/**
 * ???FUNCTION???
 *
 * @param various $fileline - Mandatory parameter: ???PARAMETER???
 * @param various $smd      - Mandatory parameter: ???PARAMETER???
 * @param various $txt      - Mandatory parameter: ???PARAMETER???
 */
function line_md5($fileline, $smd, $txt)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$chksum=$fileline+ord($smd)+strlen($txt);
	for($i=0; $i<strlen($txt); $i++)
	{
		$c=substr($txt, $i, 1);
		$ca=ord($c)%32;
		if(($c == 'a') || ($c == 'e') || ($c == 'i') || ($c == 'o') || ($c == 'u'))
			$ca *= 2;
		$chksum += $ca;
	}

	return(md5($chksum));
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function timecheck()
{
    // Display function call?
    if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

    list($usec, $sec)=explode(' ', microtime());
    return $sec.substr($usec, 1);
}
?>