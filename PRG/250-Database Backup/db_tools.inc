<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : db_tools.inc                                                 //
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

$GLOBALS['__loaded_'.basename(__FILE__)]=true;

$dbsql_array = array
(
	11 => 'sys_delete',	12 => 'sys_create', 13 => 'sys_index', 14 => '#sys_data',
	21 => 'ind_delete',	22 => 'ind_create', 23 => 'ind_index', 24 => '#ind_data', 25 => 'ind_m_delete', 26 => '!ind_m_create', 27 => '!ind_m_index', 28 => '#ind_m_data',
	31 => 'dat_delete',	32 => 'dat_create', 33 => 'dat_index', 34 => '#dat_data', 35 => '!dat_m_delete', 36 => '!dat_m_create', 37 => '!dat_m_index', 38 => '#dat_m_data',
	61 => 'fld_data',	62 => 'upd_data'
);

$del_array = array
(
	'felder'=>"id",					'masken'=>"nummer",				'maskenfelder'=>"maskenfeldid",
	'tabellen'=>"id_tabellen",		'tab_felder'=>"idintabellen",	'trans'=>"transakt_id",
	'trans_maske'=>"id_transmaske",	'trans_aktionen'=>"trans_id",
	'menu'=>"nodeid",				'texte'=>"id_referenz",			'wertetab'=>"wertetabid"
);

$db_orders = array
(
	'ma_art'=>"maart_id, id_sprache",		'masken'=>"nummer",				'maskenfelder'=>"nummer, zeile, spalte",
	'meldungen'=>"text_id, id_sprache",		'menu'=>"rootid, nodeid",		'texte'=>"id_text, id_sprache",
	'trans_maske'=>"transakt_id, zeile",	'wertetab'=>"wertetabid, id_sprache"
);

// Variablen initialisieren
$msgq			= array();	// Message-Queue (Array)

$tabellen		= array();	// Tabellendefinitionen (Array)
$spezialfelder	= array();	// Spezialfelder (Array), speichert Memo-, Float- und Integerfelder
$fieldtypes		= array();	// Field types (Array)
$views			= array();	// Views (Array)
$procedures		= array();	// Stored Procedures (Array)

// Versionscode
$short_version = (string)((int)substr($GLOBALS['fxpglobals']['dbparam']['versionscode_db'], 0, 3).(int)substr($GLOBALS['fxpglobals']['dbparam']['versionscode_db'], 3, 3).(int)substr($GLOBALS['fxpglobals']['dbparam']['versionscode_db'], 6, 3));
$dbfehler = false;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Tabellendefinitionen ermitteln
 *
 * @param string $bed_id  - Optional parameter (default = '>0'):  ???PARAMETER???
 * @param string $bed_art - Optional parameter (default = '<>3'): ???PARAMETER???
 */
function dbt_get_tabellen($bed_id='>0', $bed_art='<>3')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['tabellen'] = db_values("SELECT tabname, id_tabellen, 'pkidx' AS pkidx, tabelleart FROM tabellen WHERE id_tabellen".$bed_id." AND tabelleart".$bed_art." ORDER BY tabname", '*');
	if(fxIsArray($GLOBALS['tabellen']))
	{
		// Tabellendefinitionen einladen
		foreach($GLOBALS['tabellen'] as $tn => $tab)
		{
			$lower_tablename = strtolower($tn);

			$pkidx = trim((string)$tab['id_tabellen']);
			if(substr($pkidx, 0, 1) == '-')
				$pkidx = 'M'.substr($pkidx, 1);

			$GLOBALS['tabellen'][$tn]['pkidx'] = $pkidx;

			$felder = db_values("SELECT f.feldname, f.alias_zu, f.feldtyp, f.laenge, f.nachkomma, tf.idinfelder, tf.primaerschluessel, tf.indiziert, tf.nichtnull, tf.doppeltewerte FROM tab_felder tf, felder f WHERE tf.idintabellen=".$tab['id_tabellen']." AND f.id=tf.idinfelder AND f.mandanten_id IS NOT NULL ORDER BY positionbeitabelle", '*');
			if(is_array($felder))
			{
				$smid = false;
				foreach($felder as $fnr => $fld)
				{
					if(!$fld['feldtyp'] && $fld['alias_zu'])	// Alias-Feld
					{
						$fld['feldtyp'] = db_value("SELECT feldtyp FROM felder WHERE id=".$fld['alias_zu']." AND mandanten_id IS NOT NULL");
						$felder[$fnr]['feldtyp'] = $fld['feldtyp'];
					}
					if(!$fld['feldtyp'])
						dbt_messagequeue(1, 'Type of field not set for column "'.$tab['tabname'].'->'.$fld['feldname'].'"');

					$lower_feldname = strtolower($fld['feldname']);

					$felder[$fnr]['feldname'] = $lower_feldname;
					if($lower_feldname == 'mandanten_id')
						$smid = true;

					// 3: Memo + 15: Text 4000 Zeichen
					if(($fld['feldtyp'] == '3') || ($fld['feldtyp'] == '15'))
					{
						if(!isset($GLOBALS['spezialfelder']['memo']))
							$GLOBALS['spezialfelder']['memo']=array();
						$GLOBALS['spezialfelder']['memo'][$lower_feldname] = true;
					}
					// 6: Dezimal + 11: Betrag + 16: Budget (Intern) + 17: Budget (Extern)
					else if(($fld['feldtyp'] == '6') || ($fld['feldtyp'] == '11') || ($fld['feldtyp'] == '16') || ($fld['feldtyp'] == '17'))
					{
						if(!isset($GLOBALS['spezialfelder']['float']))
							$GLOBALS['spezialfelder']['float']=array();
						$GLOBALS['spezialfelder']['float'][$lower_feldname] = true;
					}
					// 2: Auto Inkrement + 4: Ja/Nein + 5: Ganzzahl + 12: GanzzahlLang + 14: Zeitspanne
					else if(($fld['feldtyp'] == '2') || ($fld['feldtyp'] == '4') || ($fld['feldtyp'] == '5') || ($fld['feldtyp'] == '12') || ($fld['feldtyp'] == '14'))
					{
						if(!isset($GLOBALS['spezialfelder']['integer']))
							$GLOBALS['spezialfelder']['integer']=array();
						$GLOBALS['spezialfelder']['integer'][$lower_feldname] = true;
					}
				}
				$GLOBALS['tabellen'][$lower_tablename]['mandanten_id']	= $smid;
				$GLOBALS['tabellen'][$lower_tablename]['felder']		= $felder;
			}
		}
	}
}

/**
 * Fill the global fieldtype array with the field types of all databases including additional information like type "float", "string" or "int"
 */
function dbt_get_fieldtypes()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['fieldtypes']=array();
	foreach($GLOBALS['_ftypes'] as $fti => $ftt)
	{
		$fta=array('Feldtyp'=>$ftt);
		foreach($GLOBALS['_dbtypes'] as $dbi => $dba)
			$fta[$dba['type']]=$dba['ftypes'][$fti];
		$ft=strtoupper(substr($GLOBALS['_dbtypes'][1]['ftypes'][$fti],0,3));
		// DEC(imal)			-> Type "float"
		if($ft == 'DEC')
			$fta['art']=true;
		// VAR(char) + TEX(t)	-> Type "string"
		else if(($ft == 'VAR') || ($ft == 'TEX'))
			$fta['art']="'";
		// INT(eger) + SER(ial)	-> Type "int"
		else
			$fta['art']=false;

		$GLOBALS['fieldtypes'][$fti]=$fta;
	}
//fxDebug($GLOBALS['fieldtypes'], 'fieldtypes', 0);
}

/**
 * Views ermitteln
 */
function dbt_get_views()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mxid	= 0;
	$mxpos	= 0;

	$vtr = array("\r\n"=>' ', "\r"=>' ', "\n"=>' ', '  '=>' ', ' as '=>' AS ', ' As '=>' AS ');

	// Normale Views aus Tabelle "views"
	$sql = "SELECT view_name AS name, id_view AS id, view_sql AS vsql, position FROM views ORDER BY position";
	$view_array	= db_values($sql, '*');
	if(is_array($view_array))
	{
		foreach($view_array as $va)
		{
			$sql = trim($va['vsql']);
			if(!strlen($sql))
				continue;
			if(substr($sql, -1) == ';')
				$sql = substr($sql, 0, -1);
			$GLOBALS['views'][strtoupper($va['name'])] = array('id'=>$va['id'], 'pos'=>$va['position'], 'sql'=>strtr($sql, $vtr));
		}
	}

	// Reportviews aus Tabelle "repviews"
	$sql = "SELECT repviewid AS id, view_sql AS vsql FROM repviews WHERE mandanten_id=0 AND repviewname IS NOT NULL AND repviewname>0 ORDER BY repviewid";
	$view_array	= db_values($sql, '*');
	if(is_array($view_array))
	{
		foreach($view_array as $va)
		{
			$id = (int)$va['id'];
			$vuname = 'V_FXP_'.$id;
			if(!fxIsArray($GLOBALS['views'][$vuname]))	// View existiert noch nicht
			{
				$sql = trim($va['vsql']);
				if(!strlen($sql))
					continue;
				if(substr($sql, -1) == ';')
					$sql = substr($sql, 0, -1);

				$GLOBALS['views'][$vuname] = array('id'=>1000+$id, 'pos'=>1000+$id, 'sql'=>strtr($sql, $vtr));
			}
		}
	}
}

/**
 * Stored Procedures ermitteln (wird nicht mehr benötigt)
 */
function dbt_get_procedures()
{

// Display function call?

if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }
}

/**
 * SQL-Definitionen
 *
 * @param various $dbt   - Mandatory parameter:                  ???PARAMETER???
 * @param various $afn   - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $liste - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbt_get_sqldef($dbt, $afn, $liste=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!fxIsArray($GLOBALS['fieldtypes']))
		dbt_get_fieldtypes();
//echo('$dbt='.$dbt.', $afn='.$afn.', $liste='.$liste.'<BR>'.$GLOBALS['nl']);

	// Tabellenart (Stored Procedures entfernt: 'stp'=>6)
	$tabart_array = array('sys'=>1, 'dat'=>2, 'tst'=>3, 'ind'=>4, 'viw'=>5, 'fld'=>998, 'upd'=>999);
	$tabart = $tabart_array[substr($afn, 0, 3)];
	// Datenart
	$datart_array = array('dele'=>1, 'crea'=>2, 'inde'=>3, 'data'=>4, 'm_de'=>11, 'm_cr'=>12, 'm_in'=>13, 'm_da'=>14);
	$datart = $datart_array[substr($afn, 4, 4)];
	// Mandant
	if($datart > 10)
		$mid = (int)substr($afn, strpos($afn, '_', 6)+1);
	if(!$mid)
		$mid = 0;

	$sqldata = '';
	if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
	{
		$cz_ka	= '[';
		$cz_kz	= ']';
		$cz_db	= '[dbo].[';
		$pbez	= 'PROCEDURE';
	}
	else
	{
		$cz_ka	= '';
		$cz_kz	= '';
		$cz_db	= '';
		$pbez	= 'FUNCTION';
	}
	$sk = ';'.$GLOBALS['nl_b'];

	$ecnt = 1;
	switch($datart)
	{
		case 1:		// DELETE
		case 11:	// MANDANTEN DELETE
			// Views
			if($tabart == 5)
			{
				if(!fxIsArray($GLOBALS['views']))
					dbt_get_views();

				if(fxIsArray($GLOBALS['views']))
				{
					foreach($GLOBALS['views'] as $vname => $vdata)
					{
						if($sqldata)
							$sqldata .= $GLOBALS['nl_b'];

						if($liste)
							$sqldata .= '/* fx-project: DROP VIEW #'.$ecnt.' */'.$GLOBALS['nl_b'];
						$ecnt++;

						if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
							$sqldata .= "IF EXISTS (SELECT * FROM sysobjects WHERE id=object_id(N'[dbo].[$vname]') AND OBJECTPROPERTY(id, N'IsView')=1)".$GLOBALS['nl_b'];
						$sqldata .= "DROP VIEW ".$cz_db.$vname.$cz_kz;
						if($GLOBALS['_dbtypes'][$dbt]['type'] == 'pgsql')
							$sqldata .= " CASCADE";
						$sqldata .= $sk;
					}
				}
			}

			// Stored Procedures/Functions
			else if($tabart == 6)
			{
				if(!fxIsArray($GLOBALS['procedures']))
					dbt_get_procedures();

				if(($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv') && fxIsArray($GLOBALS['procedures']))
				{
					foreach($GLOBALS['procedures'] as $pname => $pdata)
					{
						if($sqldata)
							$sqldata .= $GLOBALS['nl_b'];

						if($liste)
							$sqldata .= '/* fx-project: DROP PROCEDURE #'.$ecnt.' */'.$GLOBALS['nl_b'];
						$ecnt++;

						if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
							$sqldata .= "IF EXISTS (SELECT * FROM sysobjects WHERE id=object_id(N'[dbo].[$pname]') AND OBJECTPROPERTY(id, N'IsProcedure')=1)".$GLOBALS['nl_b'];
						$sqldata .= "DROP $pbez ".$cz_db.$pname.$cz_kz.$sk;
					}
				}
			}

			// Tabellen
			else
			{
				if(!fxIsArray($GLOBALS['tabellen']))
					dbt_get_tabellen();
				if(!fxIsArray($GLOBALS['tabellen']))
					break;

				foreach($GLOBALS['tabellen'] as $tab)
				{
					if($tab['tabelleart'] != $tabart)
						continue;

					if($datart == 11)
					{
						if($tab['mandanten_id'])
							$dt = 'CLIENT';
						else
							continue;
					}
					else
						$dt = 'TABLE';

					if($sqldata)
						$sqldata .= $GLOBALS['nl_b'];

					if($liste)
						$sqldata .= '/* fx-project: DROP '.$dt.' #'.$ecnt.' */'.$GLOBALS['nl_b'];
					$ecnt++;

					if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
						$sqldata .= "IF EXISTS (SELECT * FROM sysobjects WHERE id=object_id(N'[dbo].[".$tab['tabname']."]') AND OBJECTPROPERTY(id, N'IsUserTable')=1)".$GLOBALS['nl_b'];

					if($datart == 11)
						$sqldata .= "DELETE FROM ".$cz_db.$tab['tabname'].$cz_kz." WHERE mandanten_id=".$mid.$sk;
					else
					{
						$sqldata .= "DROP TABLE ".$cz_db.$tab['tabname'].$cz_kz;
						if($GLOBALS['_dbtypes'][$dbt]['type'] == 'pgsql')
							$sqldata .= " CASCADE";
						$sqldata .= $sk;
					}
				}
			}
		break;

		case 2:		// CREATE
		case 12:	// MANDANTEN CREATE
			// Views
			if($tabart == 5)
			{
				if(!fxIsArray($GLOBALS['views']))
					dbt_get_views();

				if(fxIsArray($GLOBALS['views']))
				{
					foreach($GLOBALS['views'] as $vname => $vdata)
					{
						if($sqldata)
							$sqldata .= $GLOBALS['nl_b'];

						if($liste)
							$sqldata .= '/* fx-project: CREATE VIEW #'.$ecnt.' */'.$GLOBALS['nl_b'];
						$ecnt++;

						$sqldata .= "CREATE VIEW ".$vname." AS".$GLOBALS['nl_b']."	".trim($vdata['sql']);
						if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
							$sqldata .= $GLOBALS['nl_b']."GO".$GLOBALS['nl_b'];
						else
							$sqldata .= $sk;
					}
				}
			}

			// Stored Procedures/Functions
			else if($tabart == 6)
			{
				if(!fxIsArray($GLOBALS['procedures']))
					dbt_get_procedures();

				if(($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv') && fxIsArray($GLOBALS['procedures']))
				{
					foreach($GLOBALS['procedures'] as $pname => $pdata)
					{
						if($sqldata)
							$sqldata .= $GLOBALS['nl_b'];

						if($liste)
							$sqldata .= '/* fx-project: CREATE PROCEDURE #'.$ecnt.' */'.$GLOBALS['nl_b'];
						$ecnt++;
						$sqldata .= trim($pdata).$sk;
					}
				}
			}
			// Tabellen
			else
			{
				if(!fxIsArray($GLOBALS['tabellen']))
					dbt_get_tabellen();
				if(!fxIsArray($GLOBALS['tabellen']))
					break;

				foreach($GLOBALS['tabellen'] as $tab)
				{
					if(($tab['tabelleart'] != $tabart) || !is_array($tab['felder']))
						continue;

					if($sqldata)
						$sqldata .= $GLOBALS['nl_b'];

					if($liste)
						$sqldata .= '/* fx-project: CREATE TABLE #'.$ecnt.' */'.$GLOBALS['nl_b'];
					$ecnt++;

					// CREATE TABLE + KLAMMER OEFFNEN
					$sqldata .= "CREATE TABLE ".$cz_db.$tab['tabname'].$cz_kz.$GLOBALS['nl_b']."(".$GLOBALS['nl_b'];

					// Alle Spalten durchgehen
					$sanz = sizeof($tab['felder']);
					$scnt = 1;
					$pkey = false;
					$cp_cnt		= 0;
					$cp_name	= '';	// PostgreSQL: CONSTRAINT... PRIMARY KEY
					$cp_spalten	= '';
					foreach($tab['felder'] as $fld)
					{
						$zusatz		= '';
						$feldtyp	= $GLOBALS['fieldtypes'][$fld['feldtyp']][$GLOBALS['_dbtypes'][$dbt]['type']];

						$kp = strpos($feldtyp, 'L(');		// DECIMAL(...)
						if($kp === false)
							$kp = strpos($feldtyp, 'R(');	// VARCHAR(...) oder NUMBER(...)
						if($kp === false)
							$kp = strpos($feldtyp, '2(');	// VARCHAR2(...)
						if($kp === false)
							$kp = strpos($feldtyp, 'T ID');	// INT IDENTITY(...)
						if($kp !== false)
						{
							$kp++;
							$zusatz = ' '.substr($feldtyp, $kp);
							$feldtyp = substr($feldtyp, 0, $kp);
						}

						// Feldtyp "Text" besonders behandeln
						if($fld['feldtyp'] == 1) // Text
						{
							$flen = $fld['laenge'];
							if(!$flen)
								$flen = 50;
							$zusatz .= " ($flen)";
						}

						// Primärschlüssel = PRIMARY KEY
						if($fld['primaerschluessel'])
						{
							$pkey = true;
							if(($GLOBALS['_dbtypes'][$dbt]['type'] == 'pgsql') && ($cp_cnt < 16))	// CONSTRAINT
							{
								if(!$cp_name)
									$cp_name = $cz_ka."PK_".$GLOBALS['short_version']."_".$tab['pkidx'].$cz_kz;
								$cp_spalten	.= ', '.$fld['feldname'];
								$cp_cnt++;
							}
						}

						// Muss in der Spalte ein Eintrag stehen, d.h. "NOT NULL" setzen
						if($fld['nichtnull'] || $fld['primaerschluessel'])
							$zusatz .= ' NOT NULL';
						else if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')	// sonst nur bei SQLServer
							$zusatz .= ' NULL';

						// Spaltendefinition schreiben
						$sqldata .= "	".$cz_ka.$fld['feldname'].$cz_kz." ".$cz_ka.$feldtyp.$cz_kz.$zusatz.$ezusatz;
						// Falls nicht letzte Spalte, dann ein Komma schreiben
						if(($scnt < $sanz) || (($GLOBALS['_dbtypes'][$dbt]['type'] == 'pgsql') && $pkey))
							$sqldata .= ',';
						$sqldata .= $GLOBALS['nl_b'];
						$scnt++;
					}
					if(($GLOBALS['_dbtypes'][$dbt]['type'] == 'pgsql') && $cp_spalten)
					{
						$sqldata .= $GLOBALS['nl_b'];
						if($cp_spalten)
						{
							$sqldata .= "	CONSTRAINT ".$cp_name." PRIMARY KEY (".substr($cp_spalten, 2).")";
							$sqldata .= $GLOBALS['nl_b'];
						}
					}

					// KLAMMER SCHLIESSEN
					$sqldata .= ")";
					if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
						$sqldata .= " ON [PRIMARY]";
					$sqldata .= $sk;
				}
			}
		break;

		case 3:		// INDEX
		case 13:	// MANDANTEN INDEX
			if(!fxIsArray($GLOBALS['tabellen']))
				dbt_get_tabellen();
			if(!fxIsArray($GLOBALS['tabellen']))
				break;

			// Primärschlüssel + Index + Sequencen + Trigger
			foreach($GLOBALS['tabellen'] as $tab)
			{
				if(($tab['tabelleart'] != $tabart) || !is_array($tab['felder']))
					continue;

				// Index
				$idx_name		= '';

				$seq_trigger	= '';
				$pri_spalten	= '';
				$idx_spalten	= '';
				$idx_array		= array();

				$i_clustered	= 0;
				$i_cnt			= 0;

				$p_clustered	= 0;

				// Alle Spalten durchgehen
				foreach($tab['felder'] as $fld)
				{
					// Auto Inkrement
					if($fld['feldtyp'] == 2)
						$seq_trigger = $fld['feldname'];

					// PostgreSQL
					if($GLOBALS['_dbtypes'][$dbt]['type'] == 'pgsql')
					{
						// Indiziert
						if($fld['indiziert'])
						{
							if($i_cnt < 16)	// max. 16 Spalten für Indizes
							{
								if(!$idx_name)
									$idx_name = "IDX_".$GLOBALS['short_version']."_".$tab['pkidx'];
								$idx_spalten .= ', '.$cz_ka.$fld['feldname'].$cz_kz;
								$idx_array[$cz_ka.$fld['feldname'].$cz_kz] = 0;
							}

							$i_cnt++;
						}
					}

					// MS-SQL
					else
					{
						// Primärschlüssel
						if($fld['primaerschluessel'])
						{
							if(!$idx_name)
								$idx_name = "IDX_".$GLOBALS['short_version']."_".$tab['pkidx'];
							// CLUSTER
							if($fld['doppeltewerte'])
							{
								if(!$p_clustered)
									$p_clustered = 1;
							}
							else
								$p_clustered = -1;
							$pri_spalten .= '	'.$cz_ka.$fld['feldname'].$cz_kz.',';
						}

						// Index
						if($fld['indiziert'])
						{
							if(!$idx_name)
								$idx_name = "IDX_".$GLOBALS['short_version']."_".$tab['pkidx'];
							// CLUSTER
							$idx_cls = 0;
							if($fld['doppeltewerte'])
							{
								if(!$i_clustered)
									$i_clustered = 1;
								$idx_cls = 1;
							}
							else
								$i_clustered = -1;
							if($i_cnt < 16)	// max. 16 Spalten für Indizes
								$idx_spalten .= ', '.$cz_ka.$fld['feldname'].$cz_kz;
							$idx_array[$cz_ka.$fld['feldname'].$cz_kz] = $idx_cls;

							$i_cnt++;
						}
					}
				}

				// Primärschlüssel schreiben
				if($pri_spalten)
				{
					if($liste)
						$sqldata .= '/* fx-project: CREATE PRIMARY KEY #'.$ecnt.' */'.$GLOBALS['nl_b'];
					$ecnt++;

					if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
					{
						$sqldata .= "ALTER TABLE ".$cz_db.$tab['tabname'].$cz_kz." WITH NOCHECK ADD CONSTRAINT ".$cz_ka."PK_".$GLOBALS['short_version']."_".$tab['pkidx'].$cz_kz." PRIMARY KEY ";
						if(($p_clustered < 1) || ($i_clustered > 0))
							$sqldata .= "NON";
						$sqldata .= "CLUSTERED".$GLOBALS['nl_b']."(".$GLOBALS['nl_b'];
					}
					else
						$sqldata .= "ALTER TABLE ".$cz_db.$tab['tabname'].$cz_kz." ADD CONSTRAINT ".$cz_ka."PK_".$GLOBALS['short_version']."_".$tab['pkidx'].$cz_kz." PRIMARY KEY".$GLOBALS['nl_b']."(".$GLOBALS['nl_b'];
					$sqldata .= str_replace(',', ','.$GLOBALS['nl_b'], substr($pri_spalten, 0, -1)).$GLOBALS['nl_b'].")";
					if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
						$sqldata .= " ON [PRIMARY]";
					$sqldata .= $sk;
				}

				// Index schreiben
				if($idx_spalten)
				{
					if($liste)
						$sqldata .= '/* fx-project: CREATE INDEX #'.$ecnt.' */'.$GLOBALS['nl_b'];
					$ecnt++;

					$idxdata = "CREATE";
					if($i_clustered > 0)
						$idxdata .= " CLUSTERED";
					$idxdata .= " INDEX ".$cz_ka.$idx_name.$cz_kz." ON ".$cz_db.$tab['tabname'].$cz_kz." (".substr($idx_spalten, 2).")";
					if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
						$idxdata .= " ON [PRIMARY]";
					$idxdata .= $sk;
//echo('<PRE>'.fxHtmlEncode($idxdata).'</PRE>'.$GLOBALS['nl']);

					$sqldata .= $idxdata;
				}
			}
		break;

		case 4:		// DATA
		case 14:	// MANDANTEN DATA
			// Mandant 1 Standarddaten
			if(($tabart == 2) && ($mid == 1))
			{
/*
				$sfn = 'mandanten_1.sql';
				$fp = fopen($sfn, 'r');
				if($fp)
				{
					$m1sql = fread($fp, filesize($sfn));
					fclose($fp);
					$m1arr = explode($GLOBALS['nl_b'], $m1sql);
					if(is_array($m1arr))
					{
						foreach($m1arr as $insert)
						{
							if(!$insert)
								continue;
							if($liste)
								$sqldata .= '/'.'* fx-project: INSERT #'.$ecnt.' *'.'/'.$GLOBALS['nl_b'];
							$ecnt++;

							$sqldata .= trim($insert).$GLOBALS['nl_b'];
						}
					}
				}
*/
			}

			// Spezialfelder
			else if($tabart == 998)
			{
				$sfa = array('memo', 'float', 'integer');
				$snl = false;
				foreach($sfa as $sf)
				{
					if(fxIsArray($GLOBALS['spezialfelder'][$sf]))
					{
						if($sqldata && $snl)
							$sqldata .= $GLOBALS['nl_b'];
						if($liste)
							$sqldata .= '/* fx-project: '.$sf.' */'.$GLOBALS['nl_b'];

						$first = true;
						foreach($GLOBALS['spezialfelder'][$sf] as $feldname => $egal)
						{
							if(!$first)
								$sqldata .= ',';
							$sqldata .= $feldname;

							$first = false;
						}
						$sqldata .= $GLOBALS['nl_b'];

						$snl = true;
					}
				}
			}

			// Update
			else if($tabart == 999)
			{
//				$sqldata = $dbt.'_'.$afn.' -> Tabellenart: '.$tabart.', Datenart: '.$datart.', Mandant: '.$mid.$GLOBALS['nl_b'];
			}

			// Tabellen
			else
			{
				if(!fxIsArray($GLOBALS['tabellen']))
					dbt_get_tabellen();
				if(!fxIsArray($GLOBALS['tabellen']))
					break;

//echo('<B>'.$dbt.': '.$afn.'</B><HR>');
				foreach($GLOBALS['tabellen'] as $tab)
				{
					if(($tab['tabelleart'] != $tabart) || !is_array($tab['felder']))
						continue;

					// Spalten ermitteln
					$identity	= false;
					$insert		= '';
					$sp_array	= '';
					$sp_cnt		= 0;
					foreach($tab['felder'] as $fld)
					{
						$fldlow = strtolower($fld['feldname']);
						if($fldlow == 'autoid')
							$identity = true;

						$insert .= ", ".$fldlow;

						$sp_array[$sp_cnt] = array('name'=>$fldlow, 'feldtyp'=>$fld['feldtyp'], 'art'=>$GLOBALS['fieldtypes'][$fld['feldtyp']]['art']);
						$sp_cnt++;
					}
//fxDebug($sp_array, 'Spalten');

					// Daten aus der Tabelle einlesen
					$sql = "SELECT * FROM ".$tab['tabname'];
					$where = false;
					if($datart == 14)
					{
						if($tab['mandanten_id'])
						{
							$sql .= " WHERE mandanten_id=".$mid;
							$where = true;
						}
						else
							continue;
					}
					else if(($tabart == 4) && ($datart == 4))	// 024_ind_data.sql
					{
						if($tab['mandanten_id'])
						{
							$sql .= " WHERE mandanten_id>0";
							$where = true;
						}
						else
							continue;
					}

					// Zusatzbedingung: bei Spezialtabellen nur positive IDs, da negative mandantenintern
					if(strlen($GLOBALS['del_array'][$tab['tabname']]))
					{
						if($tab['tabname'] != 'menu')
						{
							if($where)
								$sql .= " AND";
							else
								$sql .= " WHERE";
							$sql .= " ".$GLOBALS['del_array'][$tab['tabname']].">0";
						}
					}

					// Zusatz: bei Spezialtabellen Reihenfolge ordnen
					if(strlen($GLOBALS['db_orders'][$tab['tabname']]))
						$sql .= " ORDER BY ".$GLOBALS['db_orders'][$tab['tabname']];
//echo($sql.'<BR>');

					$daten = db_values($sql, '*');
					if(is_array($daten))
					{
//fxDebug($daten, 'Daten der Tabelle: "'.$tab['tabname'].'"');
						if($sqldata)
							$sqldata .= $GLOBALS['nl_b'];

						// Identity ON (wird nur für MS-SQL verwendet)
						if($identity)
						{
							if($liste)
								$sqldata .= '/* fx-project: IDENTITY ON #'.$ecnt.' */'.$GLOBALS['nl_b'];
							$ecnt++;

							$sqldata .= "SET IDENTITY_INSERT ".$tab['tabname']." ON".$sk;
						}

						// Transaktionsmarke setzen
						$insert = "INSERT INTO ".$tab['tabname']." (".substr($insert, 2).") VALUES (";
						foreach($daten as $dt)
						{
							if(($tab['tabname'] == 's_einstellungswerte') && ($dt['einstellungswerte_id'] == 18))
								$checkit = '!';
							else
								$checkit = '';

							$values = '';
							foreach($sp_array as $sn => $sp)
							{
								$sv = $dt[$sp['name']];
								if(!strlen($sv))
									$values .= ", NULL";
								else
								{
									// Text
									if($sp['art'] === "'")
										$values .= ", '".convert_string($sv, 'todb')."'";
									// Dezimal
									else if($sp['art'] === true)
										$values .= ", ".get_double($sv);
									// Integer
									else
										$values .= ", ".(int)$sv;
								}
							}

							if($liste)
								$sqldata .= '/* fx-project: INSERT #'.$ecnt.$checkit.' */'.$GLOBALS['nl_b'];
							$ecnt++;

							$sqldata .= $insert.substr($values, 2).")".$sk;
						}

						// Identity OFF (wird nur für MS-SQL verwendet)
						if($identity)
						{
							if($liste)
								$sqldata .= '/* fx-project: IDENTITY OFF #'.$ecnt.' */'.$GLOBALS['nl_b'];
							$ecnt++;

							$sqldata .= "SET IDENTITY_INSERT ".$tab['tabname']." OFF".$sk;
						}
					}
				}
			}
		break;
	}

	return $sqldata;
}

/**
 * dbt_get_sqldiff: Differenz zwischen zwei SQL-Definitionen ermitteln
 *
 * @param various $diff  - Mandatory parameter:                   ???PARAMETER???
 * @param various $dbt   - Mandatory parameter:                   ???PARAMETER???
 * @param various $afn   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $oktxt - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $liste - Optional parameter (default = true):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbt_get_sqldiff($diff, $dbt, $afn, $oktxt=false, $liste=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['nl_b'] = "\r\n";

	if(!is_array($diff))
		return '';

	$sqldata = '';
	if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
	{
		$cz_ka	= '[';
		$cz_kz	= ']';
		$cz_db	= '[dbo].[';
		$pbez	= 'PROCEDURE';
	}
	else
	{
		$cz_ka	= '';
		$cz_kz	= '';
		$cz_db	= '';
		$pbez	= 'FUNCTION';
	}
	$sk = ';'.$GLOBALS['nl_b'];

	// Neue Datei einladen
	if(strlen($diff['newdata']))
		$newdata = trim($diff['newdata']);
	else
	{
		$newfile = $diff['new_path'].$GLOBALS['_oss'].$dbt.$afn.'_'.$GLOBALS['dbsql_array'][$afn].'.sql';
		$nfp = fopen($newfile, 'r');
		if($nfp)
		{
			$newdata = fread($nfp, filesize($newfile));
			fclose($nfp);
		}
		else
			return('?Couldn´t load new file "'.$newfile.'"');
	}

	// Alte Datei einladen
	if(strlen($diff['olddata']))
		$olddata = trim($diff['olddata']);
	else
	{
		$oldfile = $diff['old_path'].$GLOBALS['_oss'].$dbt.$afn.'_'.$GLOBALS['dbsql_array'][$afn].'.sql';
		$ofp = fopen($oldfile, 'r');
		if($ofp)
		{
			$olddata = fread($ofp, filesize($oldfile));
			fclose($ofp);
		}
		else
			return('?Couldn´t load old file "'.$oldfile.'"');
	}

	// Tabellen aus Dateien erstellen
	$newtables = dbt_extract_tables($newdata, $dbt, true, $liste);
	$oldtables = dbt_extract_tables($olddata, $dbt, true, $liste);

	// Fehlermeldungen ausgeben, falls irgendwas schief geflaufen ist
	if(!is_array($newtables))
		return('?Couldn´t extract new table datas from file "'.$newfile.'"');
	else if(strlen($diff['newindex']))
		$newindex = trim($diff['newindex']);
	else
	{
		// Indizes laden und an Tabelle binden
		$iafn = $afn+1;
		$indexfile = $diff['new_path'].$GLOBALS['_oss'].$dbt.$iafn.'_'.$GLOBALS['dbsql_array'][$iafn].'.sql';
		$ifp = fopen($indexfile, 'r');
		if($ifp)
		{
			$indexdata = fread($ifp, filesize($indexfile));
			fclose($ifp);
		}
		else
			return('?Couldn´t load index file "'.$indexfile.'"');
		$newindex = dbt_extract_index($indexdata, $liste);
	}

	if(!is_array($oldtables))
		return('?Couldn´t extract old table datas from file "'.$oldfile.'"');
	else if(strlen($diff['oldindex']))
		$oldindex = trim($diff['oldindex']);
	else
	{
		// Indizes laden und an Tabelle binden
		$iafn = $afn+1;
		$indexfile = $diff['old_path'].$GLOBALS['_oss'].$dbt.$iafn.'_'.$GLOBALS['dbsql_array'][$iafn].'.sql';
		$ifp = fopen($indexfile, 'r');
		if($ifp)
		{
			$indexdata = fread($ifp, filesize($indexfile));
			fclose($ifp);
		}
		else
			return('?Couldn´t load index file "'.$indexfile.'"');
		$oldindex = dbt_extract_index($indexdata, $liste);
	}

	// Tabellen vergleichen
	foreach($newtables as $tabname => $spalten)
	{
//fxDebug($spalten, $tabname);
		// Die Tabelle gab es schon in der alten Version -> Spalten vergleichen
		if(fxIsArray($oldtables[$tabname]))
		{
			// Alte und neue Primärschlüssel ermitteln
			$oldpka = array();
			if(fxIsArray($oldindex) && fxIsArray($oldindex['pk'][$tabname]))
			{
				foreach($oldindex['pk'][$tabname] as $spname => $spdef)
				{
					if($spname != '#original#')
						$oldpka[$spname] = true;
				}
			}
			else if(isset($oldtables[$tabname]['#original#']))
			{
				$pkp = strpos($oldtables[$tabname]['#original#'], 'PRIMARY KEY');
				if($pkp)
				{
					$opp = strpos($oldtables[$tabname]['#original#'], '(', $pkp+11);
					$epp = strpos($oldtables[$tabname]['#original#'], ')', $pkp+11);
					if($opp && $epp)
					{
						$pe = explode(',', trim(substr($oldtables[$tabname]['#original#'], $opp+1, $epp-$opp-1)));
						foreach($pe as $pks)
							$oldpka[$pks] = true;
					}
				}
			}
			if(fxIsArray($oldpka))
			{
				ksort($oldpka);
				$oldpk = array_to_csv($oldpka);
			}
			else
				$oldpk = '';

			$newpka = array();
			if(fxIsArray($newindex) && fxIsArray($newindex['pk'][$tabname]))
			{
				foreach($newindex['pk'][$tabname] as $spname => $spdef)
				{
					if($spname != '#original#')
						$newpka[$spname] = true;
				}
			}
			else if(isset($newtables[$tabname]['#original#']))
			{
				$pkp = strpos($newtables[$tabname]['#original#'], 'PRIMARY KEY');
				if($pkp)
				{
					$opp = strpos($newtables[$tabname]['#original#'], '(', $pkp+11);
					$epp = strpos($newtables[$tabname]['#original#'], ')', $pkp+11);
					if($opp && $epp)
					{
						$pe = explode(',', trim(substr($newtables[$tabname]['#original#'], $opp+1, $epp-$opp-1)));
						foreach($pe as $pks)
							$newpka[$pks] = true;
					}
				}
			}
			if(fxIsArray($newpka))
			{
				ksort($newpka);
				$newpk = array_to_csv($newpka);
			}
			else
				$newpk = '';

			// Spezialbehandlung immer, wenn sich der Primärschlüssel verändert
			if($oldpk != $newpk)
				$change = true;
			else
			{
				$alter = array();
				$change = false;
//echo('<B><FONT COLOR="#00AA00">Table "'.$tabname.'" already exists in older version</FONT></B>...<BR>');
				foreach($spalten as $spname => $spdef)
				{
					if($spname == '#original#')
						continue;
					if($oldtables[$tabname][$spname])
					{
//echo('<B><FONT COLOR="#00AA00">Column "'.$spname.'" already exists in older version</FONT></B>...<BR>');
						if($oldtables[$tabname][$spname] != $spdef)
						{
							// Spezialbehandlung immer bei PostgreSQL
							if($GLOBALS['_dbtypes'][$dbt]['type'] == 'pgsql')
								$change=true;
							else
							{
								// Spezialbehandlung beim Ändern von MEMO-Felder
								if(substr($spdef, 0,4) == 'TEXT')
									$change=true;
								// Spezialbehandlung auch beim Ändern von Primärschlüsseln, indizierten Feldern oder Triggern
								else if(is_array($newindex) && ($newindex['pk'][$tabname][$spname] || $newindex['ix'][$tabname][$spname] || $newindex['tg'][$tabname][$spname]))
									$change=true;
							}
							$alter['change'][$spname] = $spdef;
//echo('<B><FONT COLOR="#AA0000">Column "'.$tabname.'->'.$spname.'" has changed from ['.$oldtables[$tabname][$spname].'] to ['.$spdef.']</FONT></B>...<BR>');
						}
					}
					else
					{
						$alter['add'][$spname] = $spdef;
//echo('<B><FONT COLOR="#AA0000">Column "'.$tabname.'->'.$spname.'" must be created</FONT></B>...<BR>');
					}
//fxDebug($spdef, $spname);

					// Bei Spezialbehandlung kann man die Schleife gleich verlassen
					if($change)
						break;
				}

				foreach($oldtables[$tabname] as $spname => $spdef)
				{
					if($spname == '#original#')
						continue;
					if(!$newtables[$tabname][$spname])
					{
						// Spezialbehandlung immer wenn eine Spalte gelöscht wurde
						$change = true;
						$alter['drop'][$spname] = true;
//echo('<B><FONT COLOR="#AA0000">Column "'.$tabname.'->'.$spname.'" doesn´t exist anymore</FONT></B>...<BR>');
					}

					// Bei Spezialbehandlung kann man die Schleife gleich verlassen
					if($change)
						break;
				}
			}

			// Änderungen an der Tabelle durchführen
			if($change || sizeof($alter))
			{
				if($change)
				{
					$kapos = strpos($spalten['#original#'], $GLOBALS['nl_b'].'(');
					if($kapos !== false)
					{
						$otabname = $cz_db.$tabname.$cz_kz;
						$btabname = $cz_db.$tabname.'_tmp'.$cz_kz;

						// Backup-Tabelle erstellen
						if($liste)
							$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE BACKUP TABLE #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
						$GLOBALS['counter']['counter']++;
						$sqldata .= 'CREATE TABLE '.$btabname.substr($spalten['#original#'], $kapos).$sk;

						// Alte Tabelle kopieren
						$identity = false;
						$ccn = '';
						$cco = '';
//fxDebug($newtables[$tabname], $GLOBALS['_dbtypes'][$dbt]['type'].'. '.$tabname);
						foreach($newtables[$tabname] as $spname => $spdef)
						{
							if($spname == '#original#')
								continue;
							if($spname == 'autoid')
								$identity = true;
							$ccn .= ','.$spname;
							if($oldtables[$tabname][$spname])
							{
								// Bei PostgreSQL Typecasts durchführen
								if(($GLOBALS['_dbtypes'][$dbt]['type'] == 'pgsql') && (substr($spdef, 0,3) != substr($oldtables[$tabname][$spname], 0,3)))
									$spname = 'CAST('.$spname.' AS '.$newtables[$tabname][$spname].')';

								$cco .= ','.$spname;
							}
							else
							{
								$notnull	= strpos($spdef, ' NOT');
								$ftype		= substr($spdef, 0, 3);
								if($notnull === false)
									$cco .= ',NULL AS '.$spname;
								else if($ftype == 'INT')
									$cco .= ',0 AS '.$spname;
								else if(($ftype == 'DEC') || ($ftype == 'NUM'))
									$cco .= ',0.00 AS '.$spname;
								else
									$cco .= ',\'\' AS '.$spname;
							}
						}
						if(strlen($ccn))
						{
							$ccn = substr($ccn, 1);
							$cco = substr($cco, 1);

							if($identity)
							{
								if($liste)
									$sqldata .= $GLOBALS['nl_b'].'/* fx-project: IDENTITY ON #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
								$GLOBALS['counter']['counter']++;

								$sqldata .= "SET IDENTITY_INSERT ".$btabname." ON;".$GLOBALS['nl_b'];
							}

							if($liste)
								$sqldata .= $GLOBALS['nl_b'].'/* fx-project: COPY DATA FROM OLD TO BACKUP #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
							$GLOBALS['counter']['counter']++;
							if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
							{
								$sqldata .= "IF EXISTS(SELECT * FROM ".$otabname.")".$GLOBALS['nl_b'];
								$sqldata .= "	EXEC('INSERT INTO ".$btabname." (".$ccn.") SELECT ".$cco." FROM ".$otabname." TABLOCKX');".$GLOBALS['nl_b'];
							}
							else
								$sqldata .= "INSERT INTO ".$btabname." (".$ccn.") SELECT ".$cco." FROM ".$otabname.";".$GLOBALS['nl_b'];

							if($identity)
							{
								if($liste)
									$sqldata .= $GLOBALS['nl_b'].'/* fx-project: IDENTITY OFF #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
								$GLOBALS['counter']['counter']++;

								$sqldata .= "SET IDENTITY_INSERT ".$btabname." OFF;".$GLOBALS['nl_b'];
							}
						}

						// Alte Tabelle löschen
						if($liste)
							$sqldata .= $GLOBALS['nl_b'].'/* fx-project: DROP OLD TABLE #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
						$GLOBALS['counter']['counter']++;
						if($GLOBALS['_dbtypes'][$dbt]['type'] == 'pgsql')
							$sqldata .= "DROP TABLE ".$otabname." CASCADE;".$GLOBALS['nl_b'];
						else
							$sqldata .= "DROP TABLE ".$otabname.";".$GLOBALS['nl_b'];

						// Backup-Tabelle umbenennen
						if($liste)
							$sqldata .= $GLOBALS['nl_b'].'/* fx-project: RENAME BACKUP TABLE #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
						$GLOBALS['counter']['counter']++;
						if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
							$sqldata .= "EXECUTE sp_rename '".$btabname."', '".$tabname."';".$GLOBALS['nl_b'];
						else
							$sqldata .= "ALTER TABLE ".$btabname." RENAME TO ".$tabname.";".$GLOBALS['nl_b'];

						// Sequenzen wieder setzen
						if(fxIsArray($newindex) && fxIsArray($newindex['sq']) && fxIsArray($newindex['sq'][$tabname]) && isset($newindex['sq'][$tabname]['#original#']))
						{
							if($liste)
								$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE SEQUENCE AGAIN #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
							$GLOBALS['counter']['counter']++;
							$sqldata .= $newindex['sq'][$tabname]['#original#'].$GLOBALS['nl_b'];
						}

						// Trigger wieder setzen
						if(fxIsArray($newindex) && fxIsArray($newindex['tg']) && fxIsArray($newindex['tg'][$tabname]) && isset($newindex['tg'][$tabname]['#original#']))
						{
							if($liste)
								$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE TRIGGER AGAIN #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
							$GLOBALS['counter']['counter']++;
							$sqldata .= $newindex['tg'][$tabname]['#original#'].$GLOBALS['nl_b'];
						}

						// Primärschlüssel wieder setzen
						if(fxIsArray($newindex) && fxIsArray($newindex['pk']) && fxIsArray($newindex['pk'][$tabname]) && isset($newindex['pk'][$tabname]['#original#']))
						{
							if($liste)
								$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE PRIMARY KEY AGAIN #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
							$GLOBALS['counter']['counter']++;
							$sqldata .= $newindex['pk'][$tabname]['#original#'].$GLOBALS['nl_b'];
						}

						// Index wieder setzen
						if(fxIsArray($newindex) && fxIsArray($newindex['ix']) && fxIsArray($newindex['ix'][$tabname]) && isset($newindex['ix'][$tabname]['#original#']))
						{
							if($liste)
								$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE INDEX AGAIN #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
							$GLOBALS['counter']['counter']++;
							$sqldata .= $newindex['ix'][$tabname]['#original#'].$GLOBALS['nl_b'];
						}
					}
				}
				else
				{
					ksort($alter);
//fxDebug($alter, '$alter');
					foreach($alter as $mode => $spalten)
					{
						foreach($spalten as $spname => $spdef)
						{
							if($liste)
								$sqldata .= $GLOBALS['nl_b'].'/* fx-project: ALTER OLD TABLE ('.strtoupper($mode).') #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
							$GLOBALS['counter']['counter']++;
							if($mode == 'add')
								$sqldata .= "ALTER TABLE ".$cz_db.$tabname.$cz_kz." ADD ".$cz_ka.$spname.$cz_kz." ".$spdef.$sk;
							else if($mode == 'change')
							{
								// "NOT NULL" aus Änderungs-SQL entfernen falls Spalte bereits vorher "NOT NULL"
								$notnull = strpos($spdef, ' NOT');
								if($notnull !== false)
									$spdef = trim(substr($spdef, 0, $notnull));
								if($GLOBALS['_dbtypes'][$dbt]['type'] == 'sqlsrv')
									$sqldata .= "ALTER TABLE ".$cz_db.$tabname.$cz_kz." ALTER COLUMN ".$cz_ka.$spname.$cz_kz." ".$spdef.$sk;
								else
									$sqldata .= "ALTER TABLE ".$cz_db.$tabname.$cz_kz." MODIFY ".$cz_ka.$spname.$cz_kz." ".$spdef.$sk;
							}
							else if($mode == 'drop')
								$sqldata .= "ALTER TABLE ".$cz_db.$tabname.$cz_kz." DROP COLUMN ".$cz_ka.$spname.$cz_kz.$sk;
						}
					}
				}
			}
		}

		// Die Tabelle gab es noch nicht in der alten Version -> Komplette Tabelle schreiben
		else
		{
//echo('<B><FONT COLOR="#AA0000">New Table "'.$tabname.'" must be created</FONT></B>...<BR>');
			if($liste)
				$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE NEW TABLE #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
			$GLOBALS['counter']['counter']++;
			$sqldata .= 'CREATE TABLE '.$spalten['#original#'].$sk;

			// Sequenzen setzen
			if(is_array($newindex['sq'][$tabname]))
			{
				if($liste)
					$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE SEQUENCE FOR NEW TABLE #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
				$GLOBALS['counter']['counter']++;
				$sqldata .= $newindex['sq'][$tabname]['#original#'].$GLOBALS['nl_b'];
			}

			// Trigger setzen
			if(is_array($newindex['tg'][$tabname]))
			{
				if($liste)
					$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE TRIGGER FOR NEW TABLE #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
				$GLOBALS['counter']['counter']++;
				$sqldata .= $newindex['tg'][$tabname]['#original#'].$GLOBALS['nl_b'];
			}

			// Primärschlüssel setzen
			if(is_array($newindex['pk'][$tabname]))
			{
				if($liste)
					$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE PRIMARY KEY FOR NEW TABLE #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
				$GLOBALS['counter']['counter']++;
				$sqldata .= $newindex['pk'][$tabname]['#original#'].$GLOBALS['nl_b'];
			}

			// Index setzen
			if(is_array($newindex['ix'][$tabname]))
			{
				if($liste)
					$sqldata .= $GLOBALS['nl_b'].'/* fx-project: CREATE INDEX FOR NEW TABLE #'.$GLOBALS['counter']['version'].'-'.$GLOBALS['counter']['counter'].' */'.$GLOBALS['nl_b'];
				$GLOBALS['counter']['counter']++;
				$sqldata .= $newindex['ix'][$tabname]['#original#'].$GLOBALS['nl_b'];
			}
		}
	}

	return $sqldata;
}

/**
 * dbt_extract_tables: Erstellt aus dem Textfile ein Array mit Tabellen
 *
 * @param various $str    - Mandatory parameter:                  ???PARAMETER???
 * @param various $dbt    - Mandatory parameter:                  ???PARAMETER???
 * @param various $newtab - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $liste  - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbt_extract_tables($str, $dbt, $newtab, $liste=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['nl_b'] = "\r\n";

	if(!strlen($str))
		return '';

	if($liste)
		$ottsp = 20;
	else
		$ottsp = 0;

	$tr_array = array('[dbo].' => '', '[' => '', ']' => '', '	' => '', $GLOBALS['nl_b'].'('.$GLOBALS['nl_b'] => '~', ','.$GLOBALS['nl_b'] => '*', $GLOBALS['nl_b'].$GLOBALS['nl_b'] => '*', $GLOBALS['nl_b'] => '*');
	$sp_array = array('Text' => 'TEXT', 'INTEGER' => 'INT');
	$table = array();

//echo('<PRE>'.$str.'</PRE>');
	$str_ex = explode(';', $str);
	if(is_array($str_ex))
	{
//fxDebug($str_ex);
		foreach($str_ex as $ott)
		{
			$ott = trim($ott);

			if(!strlen($ott))
				continue;

			if($liste && (substr($ott, 0, 2) != '/*'))
				continue;
			else
				$ott = trim(substr($ott, strpos($ott, 'CREATE TABLE', $ottsp)+12));

			$tt = strtr($ott, $tr_array);
//echo('<PRE>'.$tt.'</PRE>');
			$sppos = strpos($tt, '~');
			$tabname = strtolower(trim(substr($tt, 0, $sppos)));

			// nur bei neuen Tabellen brauchen wir die gesamte Tabellendefinition
			if($newtab)
				$table[$tabname]['#original#'] = $ott;

			$spalten = trim(substr($tt, $sppos+1));
			$kl = 0;
			for($i=0; $i<strlen($spalten); $i++)
			{
				$kc = substr($spalten, $i, 1);
				if($kc == '(')
					$kl++;
				else if($kc == ')')
				{
					$kl--;
					if($kl < 0)	// Letzte Klammer zu erreicht
					{
						$spalten = substr($spalten, 0, $i-1);
						break;
					}
				}
			}

			$sp_ex = explode('*', $spalten);
			if(is_array($sp_ex))
			{
				foreach($sp_ex as $spalte)
				{
					$sppos = strpos($spalte, ' ');
					$spname = strtolower(trim(substr($spalte, 0, $sppos)));
					$spdef = trim(substr($spalte, $sppos+1));
					if($spname)
					{
						if(strtolower($spname) != 'constraint')
							$table[$tabname][$spname] = $spdef;
					}
				}
			}
		}
	}

//fxDebug($table, '$table');
	return $table;
}

/**
 * dbt_extract_index: Erstellt aus dem Textfile ein Array mit Indizes, Primärschlüsseln, Sequenzen und Triggern
 *
 * @param various $indexdata - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $liste     - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbt_extract_index($indexdata, $liste=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($indexdata))
		return '';

//echo('<PRE>'.$indexdata.'</PRE>');

	$tr_array = array('[dbo].'=>'', '['=>'', ']'=>'', '('=>'', ')'=>'', ','=>'', ';'=>'', "\t"=>'', "\r\n"=>' ', "\r"=>' ', "\n"=>' ');

	$index_array = array();
	$id_ex = explode('/* fx-project', $indexdata);
	if(is_array($id_ex))
	{
		foreach($id_ex as $idx)
		{
			if(substr($idx, 0, 1) != ':')
				continue;
			$ekpos = strpos($idx, '*/');
			if($ekpos !== false)
			{
				$idx = trim(substr($idx, $ekpos+2));
				// Index (CREATE INDEX...)
				if(strtoupper(substr($idx, 0, 8)) == 'CREATE I')
				{
					$onpos = strpos($idx, ' ON ');
					if($onpos !== false)
					{
						$idxnam = trim(strtr(substr($idx, 13, $onpos-13), $tr_array));
						$tmpstr = substr($idx, $onpos+4);
						// alles hinter ) entfernen
						$delpos = strpos($tmpstr, ')');
						if($delpos !== false)
							$tmpstr = substr($tmpstr, 0, $delpos);
						$defstr = trim(strtr($tmpstr, $tr_array));
						$ds_ex = explode(' ', $defstr);
						if(is_array($ds_ex))
						{
							$tabname = strtolower(trim($ds_ex[0]));
							foreach($ds_ex as $nr => $spalte)
							{
								if(!$nr)
									$index_array['ix'][$tabname]['#original#'] = $idx;
								else
									$index_array['ix'][$tabname][strtolower(trim($spalte))] = true;
							}
						}
					}
				}
				// Sequenz (CREATE SEQUENCE...)
				else if(strtoupper(substr($idx, 0, 8)) == 'CREATE S')
				{
					$sn_ex = explode('_', substr($idx, 16));
					if(is_array($sn_ex) && fxIsArray($GLOBALS['tabellen']))
					{
						$tabnr		= trim($sn_ex[2]);
						$tabname	= '';
						foreach($GLOBALS['tabellen'] as $tn => $tab)
						{
							$cmpnr = trim((string)$tab['id_tabellen']);
							if($tabnr == $cmpnr)
							{
								$tabname = strtolower(trim($tab['tabname']));
								break;
							}
						}
						if(strlen($tabname))
							$index_array['sq'][$tabname]['#original#'] = $idx;
					}
				}
				// Trigger (CREATE TRIGGER...)
				else if(strtoupper(substr($idx, 0, 8)) == 'CREATE T')
				{
					$sn_ex = explode('_', substr($idx, 16));
					if(is_array($sn_ex) && fxIsArray($GLOBALS['tabellen']))
					{
						$tabnr		= trim($sn_ex[2]);
						$tabname	= '';
						foreach($GLOBALS['tabellen'] as $tn => $tab)
						{
							$cmpnr = trim((string)$tab['id_tabellen']);
							if($tabnr == $cmpnr)
							{
								$tabname = strtolower(trim($tab['tabname']));
								break;
							}
						}
						if(strlen($tabname))
						{
							$index_array['tg'][$tabname]['#original#'] = $idx;
							$snpos = strpos($idx, ':new.');
							$sepos = false;
							if($snpos)
								$sepos = strpos($idx, ' ', $snpos+5);
							if($snpos && $sepos)
							{
								$spalte = strtolower(trim(substr($idx, $snpos+5, $sepos-$snpos-5)));
								$index_array['tg'][$tabname][$spalte] = true;
							}
						}
					}
				}
				// Primärschlüssel (ALTER TABLE...)
				else
				{
					$tmpstr = '';
					$ekpos = strpos($idx, '(');
					if($ekpos !== false)
						$tmpstr = substr($idx, $ekpos+1);
					$tbpos = strpos($idx, 'TABLE ');
					if($tbpos !== false)
					{
						$ds_ex = explode(' ', substr($idx, $tbpos+6));
						$tabname = strtolower(trim(strtr($ds_ex[0], $tr_array)));
						$index_array['pk'][$tabname]['#original#'] = $idx;
						if(strlen($tmpstr))
						{
							// alles hinter ) entfernen
							$delpos = strpos($tmpstr, ')');
							if($delpos !== false)
								$tmpstr = substr($tmpstr, 0, $delpos);
							$defstr = trim(strtr($tmpstr, $tr_array));
							$ds_ex = explode(' ', $defstr);
							if(is_array($ds_ex))
							{
								foreach($ds_ex as $nr => $spalte)
									$index_array['pk'][$tabname][strtolower(trim($spalte))] = true;
							}
						}
					}
				}
			}
		}
	}

	return $index_array;
}

/**
 * dbt_messagequeue: Nachrichten-Queue füllen
 *
 * @param various $level     - Mandatory parameter:                   ???PARAMETER???
 * @param various $act_error - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $head      - Optional parameter (default = false):  ???PARAMETER???
 */
function dbt_messagequeue($level, $act_error, $head=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(is_array($GLOBALS['msgq']))
		$soa = sizeof($GLOBALS['msgq']);
	else
		$soa = 0;

	$GLOBALS['msgq'][$soa]['level']		= $level;
	if($head)
		$GLOBALS['msgq'][$soa]['head']	= $head;
	$GLOBALS['msgq'][$soa]['message']	= $act_error;
	$GLOBALS['msgq'][$soa]['time']		= fx_date('YmdHis', time());
}

/**
 * dbtErrorQueue: Fehler-Queue füllen
 *
 * @param various $anr       - Mandatory parameter:                   ???PARAMETER???
 * @param various $act_error - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $sql       - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $head      - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $zeit      - Optional parameter (default = false):  ???PARAMETER???
 */
function dbtErrorQueue($anr, $act_error, $sql=false, $head=false, $zeit=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Härtegrad ermitteln
	if($sql === true)	// Kein Fehler
	{
		$err_level=0;
		$sql=false;
	}
	else if(($anr == 13) || ($anr == 23) || ($anr == 33))	// Indizes
		$err_level=1;	// Warnung
	else
		$err_level=2;	// Fatal

	$GLOBALS['dbfehler']=max($GLOBALS['dbfehler'], $err_level);

	$GLOBALS['fq'][$GLOBALS['fq_cnt']]['mode']=$anr;
	$GLOBALS['fq'][$GLOBALS['fq_cnt']]['level']=$err_level;
	if($head)
		$GLOBALS['fq'][$GLOBALS['fq_cnt']]['head']=$head;
	if($sql)
		$GLOBALS['fq'][$GLOBALS['fq_cnt']]['sql']=$sql;
	$GLOBALS['fq'][$GLOBALS['fq_cnt']]['message']=$act_error;
	if(!$zeit)
		$zeit=fx_date('YmdHis', time());
	else
		$zeit=fx_date('YmdHis', $zeit);
	$GLOBALS['fq'][$GLOBALS['fq_cnt']]['time']=$zeit;

	$GLOBALS['fq_cnt']++;
}

/**
 * dbt_report: Nachrichten-Queue aufbereitet anzeigen
 *
 * @param various $txt  - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $kill - Optional parameter (default = true):  ???PARAMETER???
 */
function dbt_report($txt, $kill=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl;

	$fcol = array('AAFFAA', 'F0C0A0', 'FF8888');
	$inhalt  = '<TABLE WIDTH="100%" CELLPADDING="2" CELLSPACING="1" BGCOLOR="#000000">'.$nl;
	$inhalt .= '<TR>'.$nl;
	$inhalt .= '<TD WIDTH="100%" HEIGHT="31" COLSPAN="3" BACKGROUND="'.$GLOBALS['gfxpath'].'bgg3.png"><FONT CLASS="s3b">Report-Log</FONT></TD>'.$nl;
	$inhalt .= '</TR>'.$nl;
	if(is_array($GLOBALS['msgq']))
	{
		foreach($GLOBALS['msgq'] as $mnr => $mi)
		{
			echo('<TR>'.$nl);
			if($mi['head'])
				$head = $mnr.': '.$mi['head'];
			else
				$head = $mnr;
			$inhalt .= '<TD WIDTH="15%" VALIGN="top" BGCOLOR="#'.$fcol[$mi['level']].'"><B>'.$head.'</B></TD>'.$nl;
			$message = '<B>'.$mi['message'].'</B>';
			$inhalt .= '<TD WIDTH="80%" VALIGN="top" BGCOLOR="#FFFFFF">'.$message.'</TD>'.$nl;
			$inhalt .= '<TD WIDTH="5%" ALIGN="right" VALIGN="bottom" BGCOLOR="#EFEFEF">'.substr($mi['time'], 8, 2).':'.substr($mi['time'], 10, 2).':'.substr($mi['time'], 12, 2).'</TD>'.$nl;
			$inhalt .= '</TR>'.$nl;
		}
	}
	$inhalt .= '</TABLE>'.$nl;

	std_screen('', $inhalt);

	if($kill)
		die;
}

/**
 * -----------------------------------------------------------------------------
 * dbt_get_mtabellen
 * =============================================================================
 * Alle aktiven Mandantentabellen des Mandanten $mid ermitteln mit Ausnahmen
 * der Tabellen mit den IDs $except, z.B.
 * DMS		75=d_dokumente, 76=d_dok_verlauf, 78=d_dok_transfer_log,
 * 94=d_dok_elt_art
 * -----------------------------------------------------------------------------
 *
 * @param integer $mid          - Optional parameter (default = 0):    ???PARAMETER???
 * @param boolean $with_history - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbt_get_mtabellen($mid=0, $with_history=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$mid)
	{
		if($GLOBALS['mid'])
			$mid = $GLOBALS['mid'];
		else
			$mid = $GLOBALS['fxpglobals']['client'];
	}

	$mtab = array();

	// Standard-Ausnahmen
	// ...dms
	$except  = '94,78,76,75';
	// ...meldungen archiv
	$except .= ',136,135,143';
	// ...homepage
	$except .= ',84,168';
	// ...einstellungsdefinitionen
	$except .= ',62,63';
	// ...texte archiv
	$except .= ',124,128,139';
	// ...wertetab archiv
	$except .= ',137,134,140';

	$sql  = "SELECT t.tabname FROM tabellen t, tab_felder f";
	$sql .= " WHERE f.idintabellen=t.id_tabellen AND f.idinfelder=22 AND t.id_tabellen NOT IN (".$except.")";
	if(!$with_history)
		$sql .= " AND t.tabname NOT LIKE '%_history'";
	$sql .= " ORDER BY t.tabname";
	$tmp = db_values($sql, '*');
//fxDebug($tmp, $sql);
	if(is_array($tmp))
	{
		foreach($tmp as $t)
		{
			$tabname = strtolower($t['tabname']);
			if(db_check($tabname, 't'))
			{
				$bed = "mandanten_id=".$mid;
				$sql = "SELECT COUNT(*) FROM ".$tabname." WHERE ".$bed;
				$anz = (int)db_value($sql);
				if($anz > 0)
					$mtab[$tabname] = array('anz'=>$anz, 'bed'=>$bed);
			}
		}
	}

	$tbed = '';

	// Maskengenerator
	$sql = "SELECT nummer, name_maske FROM masken WHERE mandanten_id=".$mid;
	$mga = db_values($sql, '*');
//fxDebug($mga, $sql);
	if(is_array($mga))
	{
		$mgs = "SELECT nummer FROM masken WHERE mandanten_id=".$mid;

		// ...maskenfelder
		$tabname	= 'maskenfelder';
		$bed		= "nummer IN (".$mgs.")";
		$sql = "SELECT COUNT(*) FROM ".$tabname." WHERE ".$bed;
		$anz = (int)db_value($sql);
		if($anz > 0)
			$mtab[$tabname] = array('anz'=>$anz, 'bed'=>$bed);

		// ...tabellen
		$tnr = '';
		foreach($mga as $m)
		{
			$tbname = $m['name_maske'];
			if(substr($tbname, 0, 1) == '_')
				$tbname = substr($tbname, 1);

			$sql = "SELECT id_tabellen FROM tabellen WHERE tabelleart=2 AND ".db_like('tabname', $tbname);
			$idt = (int)db_value($sql);
			if($idt)
			{
				if(strlen($tnr))
					$tnr .= ',';
				$tnr .= $idt;
			}
		}
		if(strlen($tnr))
		{
			$tar = array
			(
				'tabellen'			=> "id_tabellen IN (".$tnr.")",
				'tab_felder'		=> "idintabellen IN (".$tnr.")",
				'trans'				=> "transakt_id IN (".$tnr.")",
				'trans_aktionen'	=> "trans_id IN (".$tnr.")",
				'trans_maske'		=> "transakt_id IN (".$tnr.")",
				'menu'				=> "trans_id IN (".$tnr.")"
			);

			foreach($tar as $tabname => $bed)
			{
				$sql = "SELECT COUNT(*) FROM ".$tabname." WHERE ".$bed;
				$anz = (int)db_value($sql);
				if($anz > 0)
					$mtab[$tabname] = array('anz'=>$anz, 'bed'=>$bed);
			}
		}

		$tbed = "(id_referenz IN (".$mgs.") AND id_ref_art=2)";
		if(strlen($tnr))
			$tbed .= " OR (id_referenz IN (".$tnr.") AND id_ref_art IN (1,6))";
	}

	// Texte
	$sql = "SELECT id FROM felder WHERE mandanten_id=".$mid;
	$fid = (int)db_value($sql);
//fxDebug($fid, $sql);
	if($fid)
	{
		if(strlen($tbed))
			$tbed .= " OR ";
		$tbed .= "(id_referenz IN (".$sql.") AND id_ref_art=3)";
	}
	if(strlen($tbed))
	{
		$tabname = 'texte';
		$sql = "SELECT COUNT(*) FROM ".$tabname." WHERE (".$tbed.")";
		$anz = (int)db_value($sql);
		if($anz > 0)
			$mtab[$tabname] = array('anz'=>$anz, 'bed'=>$tbed);
	}

//fxDebug($mtab, '$mtab');
	if(!sizeof($mtab))
		return false;

	ksort($mtab);
	return $mtab;
}

/**
 * ???FUNCTION???
 *
 * @param various $anr      - Mandatory parameter:                   ???PARAMETER???
 * @param various $sql      - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $writelog - Optional parameter (default = false):  ???PARAMETER???
 * @param integer $tline    - Optional parameter (default = 0):      ???PARAMETER???
 * @param string  $tfile    - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbtSQL($anr, $sql, $writelog=false, $tline=0, $tfile='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Trigger-Ende überspringen
	if(substr($sql, 0, 3) == 'END')
		return '';

	// Identity nur bei SQLServer setzen bzw. löschen
	$identity=strpos($sql, 'IDENTITY_INSERT');
	if($identity && ($GLOBALS['_dbtype'] != 'sqlsrv'))
		return '';

	$act_error	= '';

	if(!$tline || !strlen($tfile))
	{
		$trace = debug_backtrace();
		if(!$tline)
			$tline = $trace[0]['line'];
		if(!strlen($tfile))
			$tfile = $trace[0]['file'];
	}

	if(substr($sql, -1) == ';')
		$sql = substr($sql, 0, -1);
	else if(substr($sql, -2) == 'GO')	// bei MS-SQL (Views)
		$sql = substr($sql, 0, -4);

	// IF EXISTS-Abfragen bei MS-SQL entfernen, da sonst Fehler "changed database context to ..."
	if(substr($sql, 0, 2) == 'IF')
	{
		if(substr($anr, -1) == '1')	// DROP
		{
			$drop_pos = strpos($sql, 'DROP ');
			if($drop_pos)
				$sql = substr($sql, $drop_pos);
		}
		else						// DELETE
		{
			$delete_pos = strpos($sql, 'DELETE ');
			if($delete_pos)
				$sql = substr($sql, $delete_pos);
		}
	}

	// Bei DROP´s vorher prüfen, ob TABLE, VIEW bzw. PROCEDURE existieren
	if(substr($sql, 0, 5) == 'DROP ')
	{
		$rpla = array("[dbo]."=>'', "["=>'', "]"=>'', ";"=>'', " cascade"=>'', "table "=>'t', "view "=>'v', "procedure "=>'p');
		$csql = strtr(strtolower(substr($sql, 5)), $rpla);
		$dtype = substr($csql, 0, 1);
		$dname = substr($csql, 1);

		$cex = db_check($dname, $dtype);
		// Nichts vorhanden, was gedropt werden muss, dann gleich beenden
		if(!$cex)
			return '';
	}

	$query = @db_query($sql, $tline, $tfile);

	if($writelog)
	{
		// DROP TABLE SYS (11), IND (21), DAT (31) / VIEWS (41) / PROCEDURES (51)
		if(($anr == '11') || ($anr == '21') || ($anr == '31') || ($anr == '41') || ($anr == '51'))
			$query = true;

		// CREATE TABLE SYS (12), IND (22), DAT (32)
		if(($anr == '12') || ($anr == '22') || ($anr == '32'))
		{
			if(!is_array($GLOBALS['tabrepa']))
				$GLOBALS['tabrepa'] = array('[dbo].'=>'', '['=>'', ']'=>'');

			$tabname = trim(strtr(substr($sql, 12, strpos($sql, '(', 12) - 12), $GLOBALS['tabrepa']));
			$GLOBALS['logarray'][$anr][$tabname]['cnt']		= 0;
			$GLOBALS['logarray'][$anr][$tabname]['time']	= time();
		}
		// CREATE VIEW (42)
		else if($anr == '42')
		{
			$tabname = trim(substr($sql, 11, strpos($sql, 'AS', 11) - 11));
			$GLOBALS['logarray'][$anr][$tabname]['cnt']		= 0;
			$GLOBALS['logarray'][$anr][$tabname]['time']	= time();
		}
		// INSERT SYS (14), IND (28), DAT (38)
		else if(($anr == '14') || ($anr == '28') || ($anr == '38'))
		{
			$tabname = trim(substr($sql, 11, strpos($sql, '(', 11) - 11));
			if(!isset($GLOBALS['logarray'][$anr][$tabname]['cnt']))
			{
				$GLOBALS['logarray'][$anr][$tabname]['cnt']		= 1;
				$GLOBALS['logarray'][$anr][$tabname]['time']	= time();
			}
			else
				$GLOBALS['logarray'][$anr][$tabname]['cnt']++;
		}
	}

	// Datenbankfehler aufgetreten
	if($query == false)
	{
		if($writelog)
			$GLOBALS['logarray']['liste'][$anr]['code'] = 3;
		$act_error = db_err();
		if(!$act_error)
		{
			if($GLOBALS['_sprache'] == 1)	// Deutsch
				$act_error = 'SQL-Datenbank Fehler';
			else							// English
				$act_error = 'SQL-Database error';
		}
	}

	return $act_error;
}
?>