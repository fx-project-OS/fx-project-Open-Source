<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tools3.inc                                                   //
// Version     : 24.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2024-07-29                                                   //
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
 * Function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * nullauffuellen
 * Fuellt den String durch voranstellen von '0' auf bis zur Breite $stellen
 *
 * @param various $zahl    - Mandatory parameter: ???PARAMETER???
 * @param various $stellen - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function nullauffuellen($zahl, $stellen)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return substr((string)str_repeat('0', $stellen).$zahl, -$stellen);
}

/**
 * meldung
 * Holt einen Meldungstext der angegebenen ID in der Sprache des Benutzers und gibt ihn zurueck.
 * Existiert kein Text, wird false zurueckgegeben.
 * Autor: Steffen
 *
 * @param various $meldungid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function meldung($meldungid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Aufruf für Fehlermeldung? Ja, dann immer Farbkategorien benutzen, falls diese >= 100.00
	$err_call=false;
	if(($meldungid !== NULL) && strlen((string)$meldungid) && (substr((string)$meldungid,0,1) == 'E'))
	{
		$err_call=true;
		$meldungid=substr((string)$meldungid,1);
	}

	$meldungid=(int)$meldungid;
	if($meldungid < 1)
		return '';

	// Falls keine Sprache vorhanden, dann Sprache aus Einstellungen, falls die auch nicht vorhanden ist, dann Deutsch=1
	$msp=$GLOBALS['fxpglobals']['lang'];
	if(!$msp)
		$msp=(int)$GLOBALS['fxpglobals']['settings']['sprache'];
	if(!$msp)
		$msp=1;

	// Mandanten-ID
	$mid=$GLOBALS['fxpglobals']['client'];
	if(!$mid)
		$mid=0;

	$tid=$meldungid.'_'.$msp;

	// Prüfen, ob Wert bereits zwischengespeichert
	$use_temp_message=true;
	if($use_temp_message && fxIsArray($GLOBALS['_temp_message']) && isset($GLOBALS['_temp_message'][$tid]))
		$meldung=$GLOBALS['_temp_message'][$tid];
	else if($meldungid == 31)
	{
		$meldung=array('tabwert'=>"blank", 'position'=>0, 'id_feld'=>255, 'mandanten_id'=>0);
		if($msp == 1)
			$meldung['tabwert']='leer';
	}
	else if($meldungid == 968)
	{
		$meldung=array('tabwert'=>"Close", 'position'=>0, 'id_feld'=>364, 'mandanten_id'=>0);
		if($msp == 1)
			$meldung['tabwert']='Schließen';
	}
	else
	{
		if(!isset($GLOBALS['dberror']))
			$GLOBALS['dberror']=db_ini();
		if(($GLOBALS['dberror'] !== 'ini') && ($GLOBALS['dberror'] !== 'reini') && ($GLOBALS['dberror'] !== 'inst'))
		{
			$sql  = "SELECT tabwert,position,id_feld,mandanten_id FROM wertetab";
			$sql .= " WHERE wertetabid=".$meldungid." AND id_sprache=".$msp." AND (mandanten_id=".$mid." OR mandanten_id=0)";
			$sql .= " ORDER BY mandanten_id DESC";
			$meldung=db_values($sql);

			if(fxIsArray($meldung))
				$meldung['tabwert']=apos2quot($meldung['tabwert']);
//fxDebug($meldung,'$meldung: $sql='.$sql, 0);
		}
		else
			$meldung='';
	}

	if(!fxIsArray($meldung))
		return '';

	if($use_temp_message && (!fxIsArray($GLOBALS['_temp_message']) || !isset($GLOBALS['_temp_message'][$tid])))
		$GLOBALS['_temp_message'][$tid]=$meldung;

	// Prüfen, ob Argumente übergeben wurde
	$arg_list=func_get_args();
	$arg_cnt=sizeof($arg_list);
	$var_exists=strpos($meldung['tabwert'], '$var');
	if($var_exists !== false)
		$var_exists=true;

	// 1. Argument:                                                             
	// ------------                                                             
	//	true, 0		$var1, $var2... ersetzen durch Argumente 2, 3...            
	//	false		zusätzlich Prüfen, ob Feldname durch Literal_20 ersetzt wird
	$arg_start=1;
	$dv=-1;
	if($arg_cnt > $arg_start)
	{
		$arg1=$arg_list[$arg_start];
		if($arg1 < 0)
		{
			$dv=$arg1;
			$arg1=false;
			$var_exists=true;
		}
		$arg_start++;
	}
	else
		$arg1=true;
	$fmpos='';
	$trans_table=array();

	// Variablenersetzung vornehmen
	if($var_exists && ($arg_cnt > $arg_start))
	{
		if(fxIsArray($arg_list[$arg_start]))
		{
			if(gettype(key($arg_list[$arg_start])) == 'string')
				$trans_table=$arg_list[$arg_start];
			else
			{
			    for($i=0; $i<sizeof($arg_list[$arg_start]); $i++)
					$trans_table['$var'.($i+1)]=$arg_list[$arg_start][$i];
			}
		}
		else
		{
		    for($i=$arg_start; $i<$arg_cnt; $i++)
			{
				if(($arg1 === false) && (substr((string)$arg_list[$i], 0, 1) != '^'))
				{
					// Ist Argument leer, dann Abfrage nicht ausführen
					$Var_ID=0;
					if(isset($arg_list[$i]) && strlen((string)$arg_list[$i]) && (strpos($arg_list[$i], ' ') === false))
						$Var_ID=(int)db_value("SELECT id FROM felder WHERE mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND ".db_like('feldname', convert_string($arg_list[$i], 'todb')));
					if($Var_ID)	// wurde ein Wert gefunden, wird $varX ersetzt durch den Datenbankwert
					{
						$Var_Sp='';
						if(isset($GLOBALS['msg_lit_'.$Var_ID]) && is_string($GLOBALS['msg_lit_'.$Var_ID]) && strlen((string)$GLOBALS['msg_lit_'.$Var_ID]))
							$Var_Sp=$GLOBALS['msg_lit_'.$Var_ID];
						loadTransactionMasks();
						if(fxIsArray($GLOBALS['fx_mfields']))
						{
							foreach($GLOBALS['fx_mfields'] as $mnr => $minhalt)
							{
								// Nur spez. Maske durchsuchen?
								if(isset($GLOBALS['val_maskennummer']) && ($mnr != $GLOBALS['val_maskennummer']))
									continue;
								// Nur Masken der aktuellen Transaktion durchsuchen
								if(fxIsArray($GLOBALS['valid_masks']) && !$GLOBALS['valid_masks'][$mnr])
									continue;
								if(fxIsArray($minhalt))
								{
									foreach($minhalt as $mval)
									{
										if(fxIsArray($mval))
										{
											// Literal gefunden
											if(!$Var_Sp && ($mval['id'] == $Var_ID) && ($mval['anzeigeart'] < 5))
											{
												$Var_Sp=$mval['literal_'.($mval['anzeigeart']*5)];
											}
											// Feld gefunden
											else if((($mval['id'] == $Var_ID) || ($mval['eingabefeld'] == $Var_ID)) && ($mval['anzeigeart'] > 4))
											{
//echo(__LINE__.': Feld "'.$arg_list[$i].'" gefunden... Maskennummer='.$mnr.', Maskenart='.$GLOBALS['fx_mask'][$mnr]['maskenart'].', Anzeigeart='.$mval['anzeigeart'].'<br />');
												if(!fxIsArray($fmpos))
													$fmpos=array('mnr'=>$mnr, 'feld'=>$arg_list[$i]);
												if($Var_Sp)
													break 2;
											}
										}
									}
								}
							}
						}
						if(!strlen((string)$Var_Sp))
							$Var_Sp=db_value("SELECT literal_20 FROM texte WHERE id_ref_art=3 AND id_sprache=".$msp." AND id_referenz=".$Var_ID);
						$trans_table['$var'.($i+$dv)]='<b>'.$Var_Sp.'</b>';
					}
					else	// wurde kein Wert gefunden, wird $varX einfach durch den mitgegebenen Parameter ersetzt
						$trans_table['$var'.($i+$dv)]=$arg_list[$i];
				}
				else
				{
					if(substr((string)$arg_list[$i], 0, 1) == '^')
						$arg_list[$i]=substr((string)$arg_list[$i], 1);
					$trans_table['$var'.($i+$dv)]=$arg_list[$i];
				}
			}
		}
		if(fxIsArray($trans_table))
			$meldung['tabwert']=strtr($meldung['tabwert'], $trans_table);
	}

	// Fehlerqueue für Maske
	if(fxIsArray($fmpos) || (fxIsArray($GLOBALS['val_fmpos']) && isset($GLOBALS['val_fmpos']['feld'])))
	{
		if(fxIsArray($fmpos))
		{
			if(isset($GLOBALS['val_zeilennummer']) && ($GLOBALS['val_zeilennummer'] !== NULL) && strlen((string)$GLOBALS['val_zeilennummer']))
				$fmnr=$fmpos['mnr'].'_'.$GLOBALS['val_zeilennummer'];
			else
				$fmnr=$fmpos['mnr'];
			$fmfld=$fmpos['feld'];
		}
		else
		{
			if(isset($GLOBALS['val_zeilennummer']) && ($GLOBALS['val_zeilennummer'] !== NULL) && strlen((string)$GLOBALS['val_zeilennummer']))
				$fmnr=$GLOBALS['val_fmpos']['mnr'].'_'.$GLOBALS['val_zeilennummer'];
			else
				$fmnr=$GLOBALS['val_fmpos']['mnr'];
			$fmfld=$GLOBALS['val_fmpos']['feld'];
		}
		if(!fxIsArray($GLOBALS['fxpvars']['lookups'][$fmnr]))
			$GLOBALS['fxpvars']['lookups'][$fmnr]=array();
		$GLOBALS['fxpvars']['lookups'][$fmnr][$fmfld]=$meldung;
		$GLOBALS['fxpvars']['lookups'][$fmnr][$fmfld]['id']=$meldungid;
//fxDebug($GLOBALS['fxpvars']['lookups'],'lookups');
	}
	// Farbliche Codierung der Fehlermeldungen
	if($err_call && ($meldung['position'] >= 100000))
		return $meldung;
	else if(($meldung['position'] < 100000) || !$arg1 || ($arg1 && ($arg_cnt > 1)))
	{
		if(isset($GLOBALS['intern']) && $GLOBALS['intern'])
			$meldung['tabwert']='<span class="dintl" title="wertetab: wertetabid='.$meldungid.', id_feld='.$meldung['id_feld'].', id_sprache='.$msp.', mandanten_id='.$meldung['mandanten_id'].'" onclick="window.open(\'language.php?mfield=tabwert&ntr=-10&slng='.$GLOBALS['intern'].'&dlng='.$msp.'&pks_wertetabid='.$meldungid.'\');">'.$meldung['tabwert'].'</span>';
		return $meldung['tabwert'];
	}
	else
		return $meldung;
}

/**
 * valueCode
 * Get a lookup code and optional a lookup value of an id in the table "wertetab" in the users language.
 *
 * @param integer $id         - Mandatory parameter: "wertetabid" (or "id_feld") in the table "wertetab"
 * @param boolean $with_value - Optional parameter (default = false): Return also the lookup value
 * @param integer $ret_list   - Optional parameter (default = false): If false, single value for this is is returned, else an array of all codes/values for this field is returned
 * @param integer $lang       - Optional parameter (default = 0): Language id, or if 0 the current language is used
 *
 * @return string code (or if $with_value is true array code + value) or array of values if $ret_list is true or boolean false if entry does not exits
 */
function valueCode($id, $with_value=false, $ret_list=false, $lang=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Id
	$id=(int)$id;
	if($id < 1)
		return false;

	// Language
	$lang=(int)$lang;
	if(!$lang)
		$lang=$GLOBALS['fxpglobals']['lang'];
	if(!$lang)
		$lang=(int)$GLOBALS['fxpglobals']['settings']['sprache'];
	if(!$lang)
		$lang=7;

	// Client
	$cid=$GLOBALS['fxpglobals']['client'];
	if(!$cid)
		$cid=0;

	if($ret_list)
		$til='L';
	else
		$til='S';
	$til .= $id.'_'.$lang;

	// Use stored value, if it already exists
	if(fxIsArray($GLOBALS['_temp_codevalue']) && isset($GLOBALS['_temp_codevalue'][$til]))
		$vca=$GLOBALS['_temp_codevalue'][$til];
	// Get value from table
	else
	{
		$vca=false;
		if(!isset($GLOBALS['dberror']))
			$GLOBALS['dberror']=db_ini();
		if(($GLOBALS['dberror'] !== 'ini') && ($GLOBALS['dberror'] !== 'reini') && ($GLOBALS['dberror'] !== 'inst'))
		{
			if($ret_list)
				$fld="id_feld";
			else
				$fld="wertetabid";

			$sql  = "SELECT wertetabid, tabwert, codewert, mandanten_id FROM wertetab";
			$sql .= " WHERE ".$fld."=".$id." AND id_sprache=".$lang." AND (mandanten_id=".$cid." OR mandanten_id=0)";
			$sql .= " ORDER BY mandanten_id DESC, position, tabwert";
			$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql, 0);

			if(fxIsArray($tmp))
			{
				if(!$ret_list)
					$vca=array('code'=>$tmp[0]['codewert'], 'value'=>apos2quot($tmp[0]['tabwert']));
				else
				{
					$vca=array();
					foreach($tmp as $k => $t)
					{
						$ti=(int)$t['wertetabid'];
						if(!isset($vca[$ti]))
							$vca[$ti]=array('code'=>$t['codewert'], 'value'=>apos2quot($t['tabwert']));
					}
				}
			}
//fxDebug($vca,'$vca', 0);
		}
	}

	if(!fxIsArray($vca) || $ret_list || $with_value)
		return $vca;
	return $vca['code'];
}

/**
 * ERROR_MSG
 * ------------------------------------------------------------------------
 * Schreibt Fehlermeldungen in $GLOBALS['fxpvars']['msgarr']
 *
 * @param various $errornumber - Mandatory parameter: ???PARAMETER???
 */
function error_msg($errornumber)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Prüfen, ob Argumente übergeben wurden und Befehlszeile erstellen
	$arg_1='';
	if(fxIsArray($errornumber))
	{
		$arg_list=$errornumber;
		$errornumber=$arg_list[0];
	}
	else
		$arg_list=func_get_args();
	if(fxIsArray($GLOBALS['error_msg_array_vars']))
	{
		foreach($GLOBALS['error_msg_array_vars'] as $aln => $alv)
			$arg_list[]=$alv;
		$GLOBALS['error_msg_array_vars']=array();
	}
	$arg_cnt=sizeof($arg_list);

	$err_sign='E';
	$en1=substr((string)$errornumber,0,1);
	if(($en1 == 'N') || ($en1 == 'E'))
	{
		if($en1 == 'N')
			$err_sign='';
		$errornumber=substr((string)$errornumber,1);
	}

	if((int) $errornumber < 1)
		return;

	// 1. Argument=Array -> Felder-Fehlermeldungen
	if($arg_cnt && fxIsArray($arg_list[1]))
	{
		$arg_1=-2;
		foreach($arg_list[1] as $feld)
		{
			$fnkt='$meldung=meldung(\'E'.$errornumber.'\', -2, \''.$feld.'\'';
			for($i=2; $i<$arg_cnt; $i++)
			{
				${'arg_'.$i}=$arg_list[$i];
				$fnkt .= ', $arg_'.$i;
			}
			$fnkt .= ');';
//echo('<pre>'.fxHtmlEncode($fnkt).'</pre>');
			eval($fnkt);
		}
	}
	// Normale Fehlermeldungen
	else
	{
		$arg_1=$arg_list[1];
		$fnkt='$meldung=meldung(\'E'.$errornumber.'\'';
		for($i=1; $i<$arg_cnt; $i++)
		{
			${'arg_'.$i}=$arg_list[$i];
			$fnkt .= ', $arg_'.$i;
		}
		$fnkt .= ');';
//echo('<pre>'.fxHtmlEncode($fnkt).'</pre>');
		eval($fnkt);
	}

	// schreiben der Fehlermeldung ins Array
	if(!$arg_1 || ($arg_1 < 0))
	{
		if(fxIsArray($meldung))
		{
			$mpos=$meldung['position'];
			$mval=$meldung['tabwert'];
		}
		else
		{
			$mpos=0;
			$mval=$meldung;
		}

		// Spezialfall: keine Fehlermeldungssortierung
		if(($GLOBALS['fxpglobals']['tr'] == 36) || ($GLOBALS['fxpglobals']['tr'] == 102) || ($GLOBALS['fxpglobals']['tr'] == 189) || ($GLOBALS['fxpglobals']['tr'] == 266))
		{
			$mpos_bck=$mpos;
			$mpos=0;
		}

		if(!fxIsArray($GLOBALS['fxpvars']['msgarr'][$mpos]))
			$nxtnr=0;
		else
			$nxtnr=sizeof($GLOBALS['fxpvars']['msgarr'][$mpos]);

		// In diesen Transaktionen sollen die Fehlermeldungen nicht sortiert werden
		// => leicht abgeändertes Fehlermeldungs-Array
		if(($GLOBALS['fxpglobals']['tr'] == 36) || ($GLOBALS['fxpglobals']['tr'] == 102) || ($GLOBALS['fxpglobals']['tr'] == 189) || ($GLOBALS['fxpglobals']['tr'] == 266))
		{
			if(!isset($GLOBALS['fxpvars']['msgarr'][0]))
				$GLOBALS['fxpvars']['msgarr'][0]=array();
			if(!isset($GLOBALS['fxpvars']['msgarr'][0][$nxtnr]))
				$GLOBALS['fxpvars']['msgarr'][0][$nxtnr]=array();
			$GLOBALS['fxpvars']['msgarr'][0][$nxtnr]['nr']=$errornumber;
			$GLOBALS['fxpvars']['msgarr'][0][$nxtnr]['val']=$mval;
			$GLOBALS['fxpvars']['msgarr'][0][$nxtnr]['col']=$mpos_bck;
		}
		else
		{
			if(!isset($GLOBALS['fxpvars']['msgarr'][$mpos]))
				$GLOBALS['fxpvars']['msgarr'][$mpos]=array();
			if(!isset($GLOBALS['fxpvars']['msgarr'][$mpos][$nxtnr]))
				$GLOBALS['fxpvars']['msgarr'][$mpos][$nxtnr]=array();
			$GLOBALS['fxpvars']['msgarr'][$mpos][$nxtnr]['nr']=$errornumber;
			$GLOBALS['fxpvars']['msgarr'][$mpos][$nxtnr]['val']=$mval;
		}
//fxDebug($GLOBALS['fxpvars']['msgarr'], 'error_msg: msgarr');
	}
}

/**
 * Holt alle Benutzereinstellungen eines Benutzers aus der Datenbank
 *
 * @param integer $bid               - Optional parameter (default = 0):    ???PARAMETER???
 * @param boolean $repl_lv           - Optional parameter (default = true): ???PARAMETER???
 * @param boolean $add_emailsettings - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function benutzereinstellungen($bid=0, $repl_lv=true, $add_emailsettings=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$bid)
		$bid=$GLOBALS['fxpglobals']['user'];
	$bid=(int)$bid;
	$mid=(int)$GLOBALS['fxpglobals']['client'];

	$nachschl=array();
	$einstellungen=array();

	// 1. Schritt: Globale Einstellungen (Mandanten_ID=0, Benutzer_ID=1 (=> Sysadmin))
	$sqls  = "SELECT d.einstellungs_id, d.einstellungs_index AS idx, d.id_feld, e.einstellungswert AS wert";
	$sqls .= " FROM (s_einstellungsdef d LEFT JOIN s_einstellungswerte e ON d.einstellungs_id=e.einstellungs_id) WHERE";
	$sql   = $sqls." e.mandanten_id=0 AND e.benutzer_id=1";
	$earray=db_values($sql, '*');
//fxDebug($earray,$sql, 0);
	if(fxIsArray($earray))
	{
		foreach($earray as $ea)
		{
			$einstellungen[$ea['idx']]=$ea['wert'];
			// Wenn die Einstellung ein Nachschlage-Wert ist, merken wir uns ID_Feld, damit wir es später nachschlagen (aber erst, wenn der Wert feststeht)
			// Nur bei Sprache, MWSt., Abrechnungsart und Einheit soll die ID stehen bleiben.
			if(($ea['id_feld'] > 0) && ($ea['idx'] != 'sprache') && ($ea['idx'] != 'mwst_satz') && ($ea['idx'] != 'mwst_satz_erm') && ($ea['idx'] != 'def_abrechnart') && ($ea['idx'] != 'munit'))
				$nachschl[]=$ea['idx'];
		}
		ksort($einstellungen);
	}

	// 2. Schritt: Einstellungen Mandant 1 (Mandanten_ID=1, Benutzer_ID=1 (=> Admin Mandant 1))
	$sql=$sqls." e.mandanten_id=1 AND e.benutzer_id=1";
	$earray=db_values($sql, '*');
//fxDebug($earray,$sql, 0);
	if(fxIsArray($earray))
	{
		foreach($earray as $ea)
			$einstellungen[$ea['idx']]=$ea['wert'];
	}

	// 3. Schritt: Mandantenweite Einstellungen (Mandanten_ID, Benutzer_ID=1 (=> Admin Mandant))
	if($mid > 1)
	{
		$sql=$sqls." e.mandanten_id=".$mid." AND e.benutzer_id=1";
		$earray=db_values($sql, '*');
//fxDebug($earray,$sql, 0);
		if(fxIsArray($earray))
		{
			foreach($earray as $ea)
				$einstellungen[$ea['idx']]=$ea['wert'];
		}
	}

	// 4. Schritt: Persönliche Einstellungen (Mandanten_ID+Benutzer_ID)
	if($mid && ($bid > 1))
	{
		$sql=$sqls." e.mandanten_id=".$mid." AND e.benutzer_id=".$bid;
		$earray=db_values($sql, '*');
//fxDebug($earray,$sql, 0);
		if(fxIsArray($earray))
		{
			foreach($earray as $ea)
				$einstellungen[$ea['idx']]=$ea['wert'];
		}
	}

	// Sprache
	$einstellungen['sprache']=(int)$einstellungen['sprache'];

	// Nachschlage-Werte nachschlagen (fast immer auf Deutsch, ausser bei einigen Ausnahmen)
	if(fxIsArray($nachschl) && $repl_lv)
	{
		$luea=array('formatdatum'=>true, 'formatzeit'=>true, 'waehrung'=>true, 'd_formatdatum'=>true, 'd_formatzeit'=>true, );
//fxDebug($luea,'$luea', 0);
		foreach($nachschl as $nvalue)
		{
			if((string)$einstellungen[$nvalue] == (string)((int)$einstellungen[$nvalue]))
			{
				$luel=1;
				if(isset($luea[$nvalue]))
					$luel=$einstellungen['sprache'];
				$sql="SELECT tabwert FROM wertetab WHERE wertetabid=".(int)$einstellungen[$nvalue]." AND id_sprache=".$luel." AND mandanten_id IN (0,".$mid.")";
				$einstellungen[$nvalue]=db_value($sql);
//fxDebug($einstellungen[$nvalue],$nvalue.' -> '.$einstellungen[$nvalue].' ('.$luel.')', 0);
				if(substr((string)$einstellungen[$nvalue],0,1) == '§')
					$einstellungen[$nvalue]=substr((string)$einstellungen[$nvalue], 1);
			}
		}
	}
//fxDebug($einstellungen,'$einstellungen', 0);

	// Spezielle Anpassungen + Überprüfungen
	// - Dokumenten-Archiv
	$dalz=substr((string)$einstellungen['dok_archiv'], -1);
	if(($dalz != '\\') && ($dalz != '/'))
	{
		$datz=strpos($einstellungen['dok_archiv'], '\\');
		if($datz !== false)
			$datz='\\';
		else
			$datz='/';
		if(substr((string)$einstellungen['dok_archiv'], 0, 1) == '/')	// Linux
			$einstellungen['dok_archiv'] .= '/';
		else
			$einstellungen['dok_archiv'] .= $datz;
	}

	// - Auswahlfelder -> Ganzzahl
	$zfa=array(
		'belegnr_eindeut', 'email', 'erstejw', 'ersterwt', 'mskdsg', 'pos_button', 'save_wcheck',
		'up_gleit', 'up_krank', 'up_schulung', 'up_urlaub', 'users'
	);
	foreach($zfa as $zkey)
	{
		if(isset($einstellungen[$zkey]))
			$einstellungen['i_'.$zkey]=(int)substr((string)$einstellungen[$zkey],0,1);
	}

	// - Ganzzahlenfelder
	$zfa=array(
		'cal_maxdays_l', 'cal_maxdays_r', 'cal_maxmonths_l', 'cal_maxmonths_r', 'cal_maxquarts_l', 'cal_maxquarts_r', 'cal_maxweeks_l', 'cal_maxweeks_r',
		'd_nkstellen', 'def_abrechnart', 'framebreite', 'jhwechsel', 'listeneintraege', 'listenneu', 'logins',
		'maxdatum', 'maxliste', 'maxmatrix', 'maxpicselect', 'maxprojekte', 'melden_elter', 'melden_ma', 'mindatum', 'monatswechsel', 'mskdsg', 'munit',
		'nkstellen', 'p_toleranz', 'pos_button', 'standardbreite', 'stdzeitspanne', 'zk_ueb_jar', 'zk_ueb_monat'
	);
	foreach($zfa as $zkey)
	{
		if(isset($einstellungen[$zkey]))
			$einstellungen[$zkey]=(int)$einstellungen[$zkey];
	}

	// - Dezimalzahlenfelder
	$zfa=array(
		'maxstd_tag', 'maxstd_woche', 'mjahr', 'mmonat', 'mtag', 'mwoche', 'mwst_satz', 'mwst_satz_erm',
		'rk_frueh_ausl', 'rk_frueh_inl', 'rk_km_1954', 'rk_km_2214',
		'rk_mittagabend', 'rk_sb_abend', 'rk_sb_frueh', 'rk_sb_mittag'
	);
	foreach($zfa as $zkey)
	{
		if(isset($einstellungen[$zkey]))
			$einstellungen[$zkey]=get_double($einstellungen[$zkey]);
	}

	// - km-Pauschalfelder
	$zfa=array(
		'rk_km_1951', 'rk_km_1952', 'rk_km_1953', 'rk_km_2213', 'rk_km_3403', 'rk_km_3405'
	);
	foreach($zfa as $zkey)
	{
		if(isset($einstellungen[$zkey]))
			$einstellungen[$zkey]=get_entry($einstellungen[$zkey], 'km_float');
	}
//fxDebug($einstellungen,'$einstellungen', 0);

	// Darf der Benutzer Kosten sehen?
	$kst_adm=db_values("SELECT kosten, b_admin FROM benutzer WHERE benutzer_id=".$bid." AND mandanten_id=".$mid);
	$einstellungen['kosten']=(int)$kst_adm['kosten'];
	$einstellungen['b_admin']=(int)$kst_adm['b_admin'];

	// E-Mail-Einstellungen (Mandanteneinstellung)
	if($add_emailsettings)
	{
		$einstellungen['ems']=array();

		$sql="SELECT wertetabid, aktionscode FROM settings_email WHERE mandanten_id=0 AND kategorie_id=1 ORDER BY aktionscode";
		$pgr=get_array($sql, 'aktionscode', 'wertetabid');

		$psl=array(788=>'SO', 627=>'MA', 770=>'VG', 120=>'AG', 392=>'RP', 1386=>'SV', 121=>'PL', 1454=>'PM', 60=>'GF', 1097=>'AD');

		// 1. Schritt: Globale Einstellungen (Mandanten_ID=0)
		$sql="SELECT wertetabid, feld, aktionscode FROM settings_email WHERE mandanten_id=0 AND kategorie_id>1 ORDER BY position";
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$einstellungen['ems'][$t['wertetabid']]=array('fld'=>$t['feld'], 'ac'=>$t['aktionscode']);
				if(is_array($pgr))
				{
					foreach($pgr as $bit => $val)
					{
						if($t['aktionscode'] & $bit)
							$cs=1;
						else
							$cs=0;
						$einstellungen['ems'][$t['wertetabid']][$val]=$cs;
						$einstellungen['ems'][$t['wertetabid']][$psl[$val]]=$cs;
					}
				}
			}
		}

		// 2. Schritt: Mandantenweite Einstellungen (Mandanten_ID=$mid)
		$sql="SELECT wertetabid, aktionscode FROM settings_email WHERE mandanten_id=".$mid." AND kategorie_id>1";
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$einstellungen['ems'][$t['wertetabid']]['ac']=$t['aktionscode'];
				if(is_array($pgr))
				{
					foreach($pgr as $bit => $val)
					{
						if($t['aktionscode'] & $bit)
							$cs=1;
						else
							$cs=0;
						$einstellungen['ems'][$t['wertetabid']][$val]=$cs;
						$einstellungen['ems'][$t['wertetabid']][$psl[$val]]=$cs;
					}
				}
			}
		}
	}
	else if(fxIsArray($GLOBALS['fxpglobals']['settings']) && fxIsArray($GLOBALS['fxpglobals']['settings']['ems']))
		$einstellungen['ems']=$GLOBALS['fxpglobals']['settings']['ems'];
//fxDebug($einstellungen,'$einstellungen', 0);

	// Ermittelte Einstellungswerte zurückliefern
	return $einstellungen;
}

/**
 * ???FUNCTION???
 *
 * @param various $dbx           - Mandatory parameter:                ???PARAMETER???
 * @param various $id            - Mandatory parameter:                ???PARAMETER???
 * @param various $nv            - Mandatory parameter:                ???PARAMETER???
 * @param string  $setval_user   - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $setval_client - Optional parameter (default = ''):  ???PARAMETER???
 */
function setSetting($dbx, $id, $nv, $setval_user='', $setval_client='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen((string)$setval_user))
		$setval_user=$GLOBALS['fxpglobals']['user'];
	if(!strlen((string)$setval_client))
		$setval_client=$GLOBALS['fxpglobals']['client'];

	// Absolute ID, da ID<0 für Löschen
	$id=(int)$id;
	$aid=abs($id);

	// SQL erzeugen
	$whr=" WHERE einstellungs_id=".$aid." AND benutzer_id=".$setval_user." AND mandanten_id=".$setval_client;
	$chk="SELECT einstellungswerte_id FROM s_einstellungswerte".$whr;
	$sql="";

	$old_id=(int)db_value($chk);
	if($old_id)
	{
		$setval_id=$old_id;

		if($id < 0)
			$sql="DELETE FROM s_einstellungswerte".$whr;
		else
			$sql="UPDATE s_einstellungswerte SET einstellungswert='".convert_string($nv,'todb')."'".$whr;
	}
	else if($id >= 0)
	{
		$setval_id=id_max('s_einstellungswerte', 'einstellungswerte_id', '', '', 401);

		$sql  = "INSERT INTO s_einstellungswerte (mandanten_id, benutzer_id, einstellungswerte_id, einstellungs_id, einstellungswert)";
		$sql .= " VALUES (".$setval_client.", ".$setval_user.", ".$setval_id.", ".$aid.", '".convert_string($nv,'todb')."')";
	}
//fxDebug($sql, $chk);

	if($dbx)
		return(db_x($sql));
	return($sql);
}

/**
 * msg_sort
 * Hilfsfunktion zum sortieren des Fehlermeldungs-Arrays mittels usort (siehe rahmen.inc)
 *
 * @param various $msg1 - Mandatory parameter: ???PARAMETER???
 * @param various $msg2 - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function msg_sort($msg1, $msg2)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$len1=strpos($msg1, '|');
	$len2=strpos($msg2, '|');

	if(!$len1 && !$len2)
		return 0;
	else if(!$len1 && $len2)
		return 1;
	else if($len1 && !$len2)
		return -1;
	else if($len1 && $len2)
	{
		$pos1=substr((string)$msg1, 0, $len1);
		$pos2=substr((string)$msg2, 0, $len2);

		if($pos1==$pos2)
			return 0;
		return ($pos1 < $pos2)?1:-1;
	}
}

/**
 * link_img
 * Funktion, die einen Link erstellt, bei Übergabe der nötigen Parametern
 * $img_source= Pfad des Icons für den Link
 * $alt_text= alternativer Text des Links
 * wenn Zahl, dann bedeutet das, der Wert kommt aus der WerteTab
 * wenn String, dann wird der String, eventuell mit nötigen Zeilenumbrüchen zurückgegeben
 * $img_= optionale Parameter für Breite, Höhe und Rand des Link-Icons
 * ----------------------------------------------------------------------------------------------
 * Autor: Stefan
 *
 * @param various $img_source - Mandatory parameter:               ???PARAMETER???
 * @param various $alt_text   - Mandatory parameter:               ???PARAMETER???
 * @param integer $img_width  - Optional parameter (default = 0):  ???PARAMETER???
 * @param integer $img_height - Optional parameter (default = 0):  ???PARAMETER???
 * @param integer $img_border - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function link_img($img_source, $alt_text, $img_width=0, $img_height=0, $img_border=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$img_width	= (int)$img_width;
	$img_height	= (int)$img_height;
	$img_border	= (int)$img_border;

	// Ist alt_text eine Zahl, bedeutet das, der Text soll aus der DB kommen
	if(is_int($alt_text))
		$alt_text=db_value("SELECT tabwert FROM wertetab WHERE wertetabid=".$alt_text." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0");

	$txt='<img id="imglnk" src="'.$img_source.'" border="'.$img_border.'"';
	if($img_width)
		$txt .= ' width="'.$img_width.'"';
	if($img_height)
		$txt .= ' height="'.$img_height.'"';

	// Tooltip anzeigen
	$txt_replace=array('"'=>'', "\r\n"=>'<br />', "\r"=>'<br />', "\n"=>'<br />');
	$alt_text=strtr($alt_text, $txt_replace);
	$txt .= ' tooltip="'.$alt_text.'"';
	$txt .= ' align="top" style="cursor:help;">';

	return $txt;
}

/**
 * leave
 * Programm beenden und (Fehler-)Meldung ausgeben
 * -----------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $fm:		Anzeigemodus: true=Fehler, sonst Meldung
 * $title:	Überschrift
 * $text:	Anzeigetext
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $fm    - Mandatory parameter:                ???PARAMETER???
 * @param string  $title - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $text  - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function leave($fm, $title='', $text='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ende=true;

	if($fm == 1)
		$cls='green';
	else if($fm == 2)
		$cls='blue';
	else if($fm == 3)
		$cls='blue';
	else if($fm == 4)
		$cls='red';
	else
		$cls='black';

	if(!strlen((string)$text))
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$text='Unbekannter Fehler';
		else
			$text='Unknown error';
	}

	if(!strlen((string)$title))
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$title='Hinweis';
		else
			$title='Notice';
	}

	$inhalt  = '<table width="100%" height="100%" border="0" cellpadding="0" cellspacing="0">';
	$inhalt .= '	<tr>';
	$inhalt .= '		<td width="100%" height="100%" align="center" valign="middle">';

	if(substr((string)$text,0,1) != '#')
	{
		$inhalt .= '			<div class="maskc" style="position:relative;width:640px;">';
		$inhalt .= '				<div class="maskh maskht '.$cls.'" style="position:relative;top:0;">'.$title.'</div>';
		$inhalt .= '				<div style="position:relative;top:0;height:240px;padding:6px;margin:6px 10px;color:#333344;overflow:auto;"><table border="0" cellpadding="0" cellspacing="0" width="100%" height="100%"><tr><td height="100%" align="center">'.$text.'</td></tr></table></div>';
		$inhalt .= '			</div>';
	}
	else
		$inhalt=substr((string)$text, 1);

	$inhalt .= '		</td>';
	$inhalt .= '	</tr>';
	$inhalt .= '</table>';

	echo($inhalt);

	return $ende;
}

/**
 * Remove tags from a string
 *
 * @param string  $entry - Mandatory parameter:                  String
 * @param boolean $te    - Optional paramter (default = false):  Trim string?
 *
 * @return Processed string
 */
function removeTags($entry, $trimstring=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rentry='';
	$intag=false;
	if($entry !== NULL)
	{
		for($i=0; $i<strlen((string)$entry); $i++)
		{
			$ezp=substr((string)$entry,$i,1);
			if($intag)
			{
				if($ezp == '>')
					$intag=false;
			}
			else if($ezp == '<')
				$intag=true;
			else
				$rentry .= $ezp;
		}

		if($trimstring)
			$rentry=trim((string)$rentry);
	}

	return $rentry;
}

/**
 * Rechte für bestimmte Referenz vergeben (in Tabelle 'Rechte_Referenz')
 * Zu übergebende Variablen:
 * -------------------------
 * $ref (Nachschlagewert zu Feld 'Rechte_Ref'(1311)):
 * 35 - Reports, 37 - Vorgänge, 38 - Dokumente
 * $ref_id
 * Primärschlüssel der Referenz : RepID, Projekt_ID oder dok_id
 * $art_id
 * Benutzer_ID			wenn $art=2163 'Benutzer'
 * Benutzergrupp_ID	wenn $art=2164 'Benutzergruppen'
 * Abteilung_ID		wenn $art=2165 'Abteilungen'
 * Projekt_ID			wenn $art=1920 'Projekte'
 * $code
 * (2-ansehen, 4-anlegen, 8-ändern, 16-löschen) (Default 30=alles)
 * $art
 * 2163 'Benutzer' (Default), 2164 'Benutzergruppen',
 * 2165 'Abteilungen' oder 1920 'Projekte'
 * $mid
 * Mandanten-ID (Default=akt. Mandant)
 *
 * @param various $ref    - Mandatory parameter:                  ???PARAMETER???
 * @param various $ref_id - Mandatory parameter:                  ???PARAMETER???
 * @param various $art_id - Mandatory parameter:                  ???PARAMETER???
 * @param integer $code   - Optional parameter (default = 30):    ???PARAMETER???
 * @param integer $art    - Optional parameter (default = 2163):  ???PARAMETER???
 * @param various $mid    - Optional parameter (default = -1):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function rechte_vergeben($ref, $ref_id, $art_id, $code=30, $art=2163, $mid=-1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ref=(int)$ref;
	if(!$ref)
		return true;

	$ref_id	= (int)$ref_id;
	if(!$ref_id)
		return true;

	$art_id	= (int)$art_id;
	$code	= (int)$code;
	$art	= (int)$art;

	$mid=(int)$mid;
	if($mid < 0)
		$mid=$GLOBALS['fxpglobals']['client'];

	// Update
	if($ref < 0)
	{
		$rref=-$ref;

		$sql ="UPDATE rechte_referenz SET aktionscode=".$code;
		$sql .= " WHERE id_referenz=".$ref_id." AND rechte_ref=".$rref." AND mandanten_id=".$mid." AND benutzergr_id=".$art_id." AND gruppeart=".$art;
	}
	// Neuanlage
	else
	{
		$rref=$ref;
		$id=id_max('rechte_referenz', 'rechte_referenzid');

		$sql ="INSERT INTO rechte_referenz";
		$sql .= " (mandanten_id, rechte_referenzid, rechte_ref, id_referenz, gruppeart, benutzergr_id, aktionscode) ";
		$sql .= "VALUES";
		$sql .= " (".$mid.", ".$id.", ".$rref.", ".$ref_id.", ".$art.", ".$art_id.", ".$code.")";
	}
//fxDebug($sql, '$sql');

	$errf=db_x($sql, 0);

	return $errf;
}

/**
 * Rechte für bestimmte Referenz löschen (in Tabelle 'Rechte_Referenz')
 * Variablen (siehe function rechte_vergeben)
 *
 * @param various $ref    - Mandatory parameter:                  ???PARAMETER???
 * @param various $ref_id - Mandatory parameter:                  ???PARAMETER???
 * @param string  $art_id - Optional parameter (default = ''):    ???PARAMETER???
 * @param integer $art    - Optional parameter (default = 2163):  ???PARAMETER???
 * @param various $mid    - Optional parameter (default = -1):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function rechte_loeschen($ref, $ref_id, $art_id='', $art=2163, $mid=-1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ref=(int)$ref;
	if(!$ref)
		return true;

	$ref_id	= (int)$ref_id;
	if(!$ref_id)
		return true;

	$art_id	= (int)$art_id;
	$art	= (int)$art;

	$mid=(int)$mid;
	if($mid < 0)
		$mid=$GLOBALS['fxpglobals']['client'];

	$sql="DELETE FROM rechte_referenz WHERE id_referenz=".$ref_id." AND rechte_ref=".$ref." AND mandanten_id=".$mid;
	if($art_id && $art)
		$sql .= " AND benutzergr_id=".$art_id." AND gruppeart=".$art;
//fxDebug($sql, '$sql');

	$errf=db_x($sql);

	return $errf;
}

/**
 * DM_WINDOWS
 * ----------------------------------------------------------------------------------------------
 * Datei-Manager-Oberfläche in HTML aufbereiten
 *
 * @param various $fn - Mandatory parameter:                ???PARAMETER???
 * @param string  $ad - Optional parameter (default = ''):  ???PARAMETER???
 */
function dm_window($fn, $ad='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$backupform=$GLOBALS['_form'];
	$GLOBALS['_form']='';

	// Ungültige Filenamen mit Prozentzeichen ersetzen
	$pp=strpos($fn, '%');
	if($pp !== false)
		$fn=substr((string)$fn, 0, $pp);

	// Währung
	$currency=$GLOBALS['waehrung'];
	if(!strlen((string)$currency))
		$currency=$GLOBALS['fxpglobals']['settings']['waehrung'];

	// Dateiname
	if(!strlen((string)$ad))
		$ad=fx_date('YmdHis').$GLOBALS['fxpglobals']['person'];

	// Breite
	$width=373;

	// Title + Literal
	$title=meldung(1578,false);			// Dateimanager
	$close_literal=meldung(968,true);	// Schließen

	// Inhalt
	$showempty=true;
	$inhalt='';
	// *** ZIP ***
	if(substr((string)$fn, -4) == '.zip')
	{
		$fn=substr((string)$fn, 0,-4);

		$inhalt .= '<tr>';

		// Linke Spalte
		$dh=48;
		$desc='Compr. File Archive Format';
		$ftxt='File';
		if($GLOBALS['fxpglobals']['lang'] == 1)
		{
			$desc='Komprim. Dateiarchivformat';
			$ftxt='Datei';
		}
		$inhalt .= '<td width="240" nowrap align="left">';
		$inhalt .= '<div class="fmtype" style="height:{dh}px;">';
		$inhalt .= '<img src="'.$GLOBALS['gfxpath'].'dm_zip_c_48x48.png" border="0" align="left"><div style="position:absolute;left:80px;"><b style="font-size:16pt;">ZIP</b><br /><b style="font-size:7pt;">'.$desc.'</b></div>';
		$inhalt .= '</td>';

		// Rechte Spalte
		$inhalt .= '<td>';
		$inhalt .= '<table width="100%" border="0" cellspacing="6" cellpadding="0">';
		$inhalt .= '<tr>';

		// ZIP-Datei downloaden
		$inhalt .= '<td width="'.$width.'" nowrap align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=1&type=zip&filename='.$fn.'&dat='.$ad, $fn.'.zip');
		$inhalt .= '<div class="fmaction">'.meldung(1615,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_dm.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		$inhalt .= '<td width="'.$width.'" align="center"><b>'.$ftxt.':</b>&nbsp;<font class="darkgrey">&quot;'.$fn.'.zip&quot;</font></td>';

		$inhalt .= '</tr>';

		$inhalt .= '</table>';
		$inhalt .= '</td>';
		$inhalt .= '</tr>';

		$inhalt=str_replace('{dh}',$dh,$inhalt);
		$showempty=false;
	}

	// *** TEXT ***
	if((substr((string)$fn, -4) == '.txt') && strlen((string)$GLOBALS['txtstr']))
	{
		$fn=substr((string)$fn, 0, -4);

		// TEXT-Dokument speichern und Eintrag im Download-Manager anzeigen
		$fs=$GLOBALS['tmppath'].'~'.$ad.'.txt';
		fxSave($fs, $GLOBALS['txtstr']);

		$inhalt .= '<tr>';

		// Linke Spalte
		$dh=48;
		$inhalt .= '<td width="240" nowrap align="left">';
		$inhalt .= '<div class="fmtype" style="height:{dh}px;">';
		$inhalt .= '<img src="'.$GLOBALS['gfxpath'].'dm_txt_c_48x48.png" border="0" align="left"><div style="position:absolute;left:80px;"><b style="font-size:16pt;">TEXT</b><br /><b class="darkgrey" style="font-size:7pt;">&quot;'.$fn.'.txt&quot;</b></div>';
		$inhalt .= '</td>';

		// Rechte Spalte
		$inhalt .= '<td>';
		$inhalt .= '<table width="100%" border="0" cellspacing="6" cellpadding="0">';
		$inhalt .= '<tr>';

		// TEXT-Datei downloaden
		$inhalt .= '<td width="'.$width.'" nowrap align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=1&type=txt&filename='.$fn.'&dat='.$ad, $fn.'.txt');
		$inhalt .= '<div class="fmaction">'.meldung(1619,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_dm.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		// TEXT-Datei im Browser öffnen
		$inhalt .= '<td width="'.$width.'" align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=2&type=txt&filename='.$fn.'&dat='.$ad);
		$inhalt .= '<div class="fmaction">'.meldung(1620,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_bw.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		$inhalt .= '</tr>';

		$inhalt .= '</table>';
		$inhalt .= '</td>';
		$inhalt .= '</tr>';

		$inhalt=str_replace('{dh}',$dh,$inhalt);
		$showempty=false;
	}

	// *** HTML ***
	if(isset($GLOBALS['html_data']) && strlen((string)$GLOBALS['html_data']))
	{
		// HTML-Dokument speichern und Eintrag im Download-Manager anzeigen
		$fs=$GLOBALS['tmppath'].'~'.$ad.'.html';
		fxSave($fs, $GLOBALS['html_data']);

		$inhalt .= '<tr>';

		// Linke Spalte
		$dh=48;
		$inhalt .= '<td width="240" nowrap align="left">';
		$inhalt .= '<div class="fmtype" style="height:{dh}px;">';
		$inhalt .= '<img src="'.$GLOBALS['gfxpath'].'dm_html_c_48x48.png" border="0" align="left"><div style="position:absolute;left:80px;"><b style="font-size:16pt;">HTML</b><br /><b style="font-size:7pt;">Hypertext Markup Language</b></div>';
		$inhalt .= '</td>';

		// Rechte Spalte
		$inhalt .= '<td>';
		$inhalt .= '<table width="100%" border="0" cellspacing="6" cellpadding="0">';
		$inhalt .= '<tr>';

		// HTML-Datei downloaden
		$inhalt .= '<td width="'.$width.'" nowrap align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=1&type=html&filename='.$fn.'&dat='.$ad, $fn.'.html');
		$inhalt .= '<div class="fmaction">'.meldung(1599,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_dm.png" border="0"></div></a>';
		$inhalt .= '</td>';

		// HTML-Datei im Browser öffnen
		$inhalt .= '<td width="'.$width.'" align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=2&type=html&filename='.$fn.'&dat='.$ad);
		$inhalt .= '<div class="fmaction">'.meldung(1600,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_bw.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		$inhalt .= '</tr>';

		$inhalt .= '</table>';
		$inhalt .= '</td>';
		$inhalt .= '</tr>';

		$inhalt=str_replace('{dh}',$dh,$inhalt);
		$showempty=false;
	}

	// *** CSV ***
	// nicht bei 262: Personal-Projekt-Einsatzplan + 263: Projekt-Personal-Einsatzplan
	if(isset($GLOBALS['csv_array']) && (($GLOBALS['fxpglobals']['tr'] != 262) && ($GLOBALS['fxpglobals']['tr'] != 263)))
	{
		$csv='';
//fxDebug($GLOBALS['csv_array'],'csv_array');
		$csvs=';';
		$csvi='"';
		// Columns
		$lcol=(int)$GLOBALS['csv_array']['lcol'];
		// Headers
		if(fxIsArray($GLOBALS['csv_array']['headers']))
		{
			for($c=0; $c<=$lcol; $c++)
			{
				if($c)
					$csv .= $csvs;
				if(isset($GLOBALS['csv_array']['headers'][$c]))
					$csv .= $csvi.str_replace('"', '', $GLOBALS['csv_array']['headers'][$c]).$csvi;
				if(fxIsArray($GLOBALS['csv_array']['units']) && isset($GLOBALS['csv_array']['units'][$c]))
					$csv .= $csvs;
			}
			$csv .= $GLOBALS['nl'];
		}
		// Lines
		if(fxIsArray($GLOBALS['csv_array']['lines']))
		{
			foreach($GLOBALS['csv_array']['lines'] as $l => $ca)
			{
				for($c=0; $c<=$lcol; $c++)
				{
					if($c)
						$csv .= $csvs;
					if(isset($ca[$c]))
					{
						$ct=$ca[$c];
						if(is_string($ct))
						{
							if(isset($GLOBALS['nlrep']))
								$ct=str_ireplace($GLOBALS['nlrep'],' ',$ct);
							$csv .= $csvi.str_replace('"', '', $ct).$csvi;
						}
						else if(is_double($ct))
							$csv .= get_entry($ct,'dezimal');
						else
							$csv .= $ct;
					}
					if(fxIsArray($GLOBALS['csv_array']['units']) && isset($GLOBALS['csv_array']['units'][$c]))
						$csv .= $csvs.$csvi.$GLOBALS['csv_array']['units'][$c].$csvi;
				}
				$csv .= $GLOBALS['nl'];
			}
		}
//echo('<pre>'.fxHtmlEncode($csv).'</pre>');

		// CSV-Dokument speichern und Eintrag im Download-Manager anzeigen
		$fs=$GLOBALS['tmppath'].'~'.$ad.'.csv';
		fxSave($fs, $csv);

		$inhalt .= '<tr>';

		// Linke Spalte
		$dh=48;
		$inhalt .= '<td width="240" nowrap align="left">';
		$inhalt .= '<div class="fmtype" style="height:{dh}px;">';
		$inhalt .= '<img src="'.$GLOBALS['gfxpath'].'dm_csv_c_48x48.png" border="0" align="left"><div style="position:absolute;left:80px;"><b style="font-size:16pt;">CSV</b><br /><b style="font-size:7pt;">Comma Separated Values</b></div>';
		$inhalt .= '</td>';

		// Rechte Spalte
		$inhalt .= '<td>';
		$inhalt .= '<table width="100%" border="0" cellspacing="6" cellpadding="0">';
		$inhalt .= '<tr>';

		// CSV-Datei downloaden
		$inhalt .= '<td width="'.$width.'" nowrap align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=1&type=csv&filename='.$fn.'&dat='.$ad, $fn.'.csv');
		$inhalt .= '<div class="fmaction">'.meldung(1579,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_dm.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		// CSV-Datei im Browser öffnen
		$inhalt .= '<td width="'.$width.'" align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=2&type=csv&filename='.$fn.'&dat='.$ad);
		$inhalt .= '<div class="fmaction">'.meldung(1580,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_bw.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		$inhalt .= '</tr>';

		// Spezial-CSV-Report
		if(strlen((string)$GLOBALS['fxptdata']['stime']) && ((int)substr((string)$GLOBALS['fxptdata']['stime'],0,3) == $GLOBALS['fxpglobals']['tr']))
		{
			$cad=substr((string)$GLOBALS['fxptdata']['stime'],3).$GLOBALS['fxpglobals']['person'];
			$cfn=$GLOBALS['tmppath'].'~'.$cad.'.csv';
			if(file_exists($cfn))
			{
				$dh=136;
				$inhalt .= '<tr>';

				// Special CSV-Datei downloaden
				$inhalt .= '<td width="'.$width.'" nowrap align="center">';
				$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=1&type=csv&filename=Special'.$fn.'&dat='.$cad, 'Special-'.$fn.'.cvs');
				$inhalt .= '<div class="fmaction">Special '.meldung(1579,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_dm.png" border="0"></div>';
				$inhalt .= '</a>';
				$inhalt .= '</td>';

				// Special CSV-Datei im Browser öffnen
				$inhalt .= '<td width="'.$width.'" align="center">';
				$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=2&type=csv&filename=Special'.$fn.'&dat='.$cad);
				$inhalt .= '<div class="fmaction">Special '.meldung(1580,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_bw.png" border="0"></div>';
				$inhalt .= '</a>';
				$inhalt .= '</td>';

				$inhalt .= '</tr>';
			}
			else
				unset($GLOBALS['fxptdata']['stime']);
		}

		$inhalt .= '</table>';
		$inhalt .= '</td>';
		$inhalt .= '</tr>';

		$inhalt=str_replace('{dh}',$dh,$inhalt);
		$showempty=false;
	}

	// *** Excel ***
	if(fxIsArray($GLOBALS['excel_array']))
	{
//fxDebug($GLOBALS['excel_array'],'excel_array: $fn='.$fn.', $ad='.$ad);

		$inhalt .= '<tr>';

		// Linke Spalte
		$dh=48;
		$inhalt .= '<td width="240" nowrap align="left">';
		$inhalt .= '<div class="fmtype" style="height:{dh}px;">';
		if($GLOBALS['_elinks'])
			$inhalt .= '<a href="https://products.office.com/de-de/excel" target="_blank">';
		$inhalt .= '<img src="'.$GLOBALS['gfxpath'].'dm_xls_c_48x48.png" border="0" align="left"><div style="position:absolute;left:80px;"><b style="font-size:16pt;">Excel</b><br /><b style="font-size:7pt;">Microsoft &reg;</b></div>';
		if($GLOBALS['_elinks'])
			$inhalt .= '</a>';
		$inhalt .= '</td>';

		// Rechte Spalte
		$inhalt .= '<td>';
		$inhalt .= '<table width="100%" border="0" cellspacing="6" cellpadding="0">';
		$inhalt .= '<tr>';

		// Excel-Datei downloaden
		$inhalt .= '<td width="'.$width.'" nowrap align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=1&type='.$GLOBALS['excel_array']['type'].'&filename='.$fn.'&dat='.$ad, $fn.'.'.$GLOBALS['excel_array']['type']);
		$inhalt .= '<div class="fmaction">'.meldung(3200,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_dm.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		// Excel-Datei im Browser öffnen
		$inhalt .= '<td width="'.$width.'" align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=2&type='.$GLOBALS['excel_array']['type'].'&filename='.$fn.'&dat='.$ad);
		$inhalt .= '<div class="fmaction">'.meldung(3201,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_bw.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		$inhalt .= '</tr>';
		$inhalt .= '</table>';
		$inhalt .= '</td>';

		$inhalt .= '</tr>';

		$inhalt=str_replace('{dh}',$dh,$inhalt);

		$showempty=false;
	}

	// *** PDF ***
	if(isset($GLOBALS['pdf_data']) && strlen((string)$GLOBALS['pdf_data']))
	{
		// PDF-Dokument speichern und Eintrag im Download-Manager anzeigen
		$fs=$GLOBALS['tmppath'].'~'.$ad.'.pdf';
		fxSaveBin($fs, $GLOBALS['pdf_data']);

		$inhalt .= '<tr>';

		// Linke Spalte
		$dh=288;
		$inhalt .= '<td width="240" nowrap align="left">';
		$inhalt .= '<div class="fmtype" style="height:{dh}px;">';	// <b style="font-size:14pt;">PDF</b><br /><br /><b style="font-size:12pt;">P</b>ortable <b style="font-size:12pt;">D</b>ocument <b style="font-size:12pt;">F</b>ormat<br /><br /><br /><br /><b style="font-size:9pt;">Adobe Acrobat Reader &reg;</b><br />';
		if($GLOBALS['_elinks'])
			$inhalt .= '<a href="https://www.adobe.de/products/acrobat/readstep.html" target="_blank">';
		$inhalt .= '<img src="'.$GLOBALS['gfxpath'].'dm_pdf_c_48x48.png" border="0" align="left"><div style="position:absolute;left:80px;"><b style="font-size:16pt;">PDF</b><br /><b style="font-size:7pt;">Adobe Acrobat Reader &reg;</b></div>';
		if($GLOBALS['_elinks'])
			$inhalt .= '</a>';
		$inhalt .= '</div>';
		$inhalt .= '</td>';

		// Rechte Spalte
		$inhalt .= '<td>';
		$inhalt .= '<table width="100%" border="0" cellspacing="6" cellpadding="0">';

		// PDF-Einstellungen
		if(!$GLOBALS['pdfloaded'])
		{
			$pdffac=0.35273159145;

			// Minimum-Werte (in mm)
			$min_height=210.0;
			$min_height=297.0;

			$inhalt .= '<tr>';
			$inhalt .= '<td nowrap align="center" style="text-decoration:none;" colspan="2">';
			$sadd='';
			if($_GET['pdfs_button'])
				$sadd='box-shadow:1px 1px 6px #006b9f;';
			$inhalt .= '<div class="fmpdfparam" style="text-align:left;'.$sadd.'">';
			$inhalt .= '<table width="100%" border="0" cellpadding="0" cellspacing="2">';

			// ...Formular
			if($GLOBALS['fxpglobals']['tr'] == 114)	// Leistungsnachweis
			{
				$inhalt .= '<tr>';
				$lit=get_text(1874,'10');	// Formular
				$inhalt .= '<td><b>'.$lit.':</b></td>';
				$inhalt .= '<td colspan="7">';
				$inhalt .= fieldSelect('pdfs_form',$GLOBALS['pdf_array']['form'],396);
				$inhalt .= '</td>';
				$inhalt .= '</tr>';

				$inhalt .= '<tr>';
				$inhalt .= '<td colspan="8"><hr class="fxhr"></td>';
				$inhalt .= '</tr>';
			}

			// ...Seitenformate
			$inhalt .= '<tr>';
			if($GLOBALS['pdf_array']['oheight'] > $GLOBALS['pdf_array']['owidth'])
			{
				$sside=round($GLOBALS['pdf_array']['owidth']*$pdffac);
				$lside=round($GLOBALS['pdf_array']['oheight']*$pdffac);
			}
			else
			{
				$sside=round($GLOBALS['pdf_array']['oheight']*$pdffac);
				$lside=round($GLOBALS['pdf_array']['owidth']*$pdffac);
			}
			$GLOBALS['pdfs_format_arg']=array('sside'=>$sside,'lside'=>$lside);
			$selval=$sside.'x'.$lside;
			$lit=meldung(628);	// Seitenformate
			$inhalt .= '<td><b>'.$lit.':</b></td>';
			$inhalt .= '<td colspan="7">';
			$inhalt .= fieldSelect('pdfs_format',$selval,396);
			$inhalt .= '</td>';
			$inhalt .= '</tr>';

			// ...Seitenbreite + Seitenhöhe
			$inhalt .= '<tr>';
			$lit=meldung(1627);	// Seitenbreite
			$inhalt .= '<td><b>'.$lit.':</b></td>';
			$inhalt .= '<td>'.fieldText('pdfs_width',get_entry($GLOBALS['pdf_array']['owidth']*$pdffac, 'dezimal'),48,8,'right').'</td>';
			$inhalt .= '<td nowrap>mm&nbsp;&nbsp;<i>(min.&nbsp;'.get_entry($min_width, 'dezimal').'&nbsp;mm)</i></td>';
			$inhalt .= '<td width="20">&nbsp;</td>';
			$lit=meldung(1628);	// Seitenhöhe
			$inhalt .= '<td><b>'.$lit.':</b></td>';
			$inhalt .= '<td>'.fieldText('pdfs_height',get_entry($GLOBALS['pdf_array']['oheight']*$pdffac, 'dezimal'),48,8,'right').'</td>';
			$inhalt .= '<td colspan="2" nowrap>mm&nbsp;&nbsp;<i>(min.&nbsp;'.get_entry($min_height, 'dezimal').'&nbsp;mm)</i></td>';
			$inhalt .= '</tr>';

			$inhalt .= '<tr>';
			$inhalt .= '<td colspan="8"><hr class="fxhr"></td>';
			$inhalt .= '</tr>';

			// ...Linker Rand + Oberer Rand
			$inhalt .= '<tr>';
			$lit=meldung(1629);	// Linker Rand
			$inhalt .= '<td><b>'.$lit.':</b></td>';
			$inhalt .= '<td>'.fieldText('pdfs_lmargin',get_entry($GLOBALS['pdf_array']['lmargin']*$pdffac, 'dezimal'),48,8,'right').'</td>';
			$inhalt .= '<td nowrap>mm&nbsp;&nbsp;<i>(min.&nbsp;'.get_entry(7.00, 'dezimal').'&nbsp;mm)</i></td>';
			$inhalt .= '<td>&nbsp;</td>';
			$lit=meldung(1630);	// Oberer Rand
			$inhalt .= '<td><b>'.$lit.':</b></td>';
			$inhalt .= '<td>'.fieldText('pdfs_tmargin',get_entry($GLOBALS['pdf_array']['tmargin']*$pdffac, 'dezimal'),48,8,'right').'</td>';
			$inhalt .= '<td colspan="2" nowrap>mm&nbsp;&nbsp;<i>(min.&nbsp;'.get_entry(7.00, 'dezimal').'&nbsp;mm)</i></td>';
			$inhalt .= '</tr>';

			// ...Rechter Rand + Unterer Rand
			$inhalt .= '<tr>';
			$lit=meldung(1631);	// Rechter Rand
			$inhalt .= '<td><b>'.$lit.':</b></td>';
			$inhalt .= '<td>'.fieldText('pdfs_rmargin',get_entry($GLOBALS['pdf_array']['rmargin']*$pdffac, 'dezimal'),48,8,'right').'</td>';
			$inhalt .= '<td nowrap>mm&nbsp;&nbsp;<i>(min.&nbsp;'.get_entry(7.00, 'dezimal').'&nbsp;mm)</i></td>';
			$inhalt .= '<td>&nbsp;</td>';
			$lit=meldung(1632);	// Unterer Rand
			$inhalt .= '<td><b>'.$lit.':</b></td>';
			$inhalt .= '<td>'.fieldText('pdfs_bmargin',get_entry($GLOBALS['pdf_array']['bmargin']*$pdffac, 'dezimal'),48,8,'right').'</td>';
			$inhalt .= '<td colspan="2" nowrap>mm&nbsp;&nbsp;<i>(min.&nbsp;'.get_entry(7.00, 'dezimal').'&nbsp;mm)</i></td>';
			$inhalt .= '</tr>';

			$inhalt .= '<tr>';
			$inhalt .= '<td colspan="8"><hr class="fxhr"></td>';
			$inhalt .= '</tr>';

			// ...Skalierungsfaktor, Querformat + In Graustufen
			$inhalt .= '<tr>';
			$lit=meldung(1633);	// Skalierungsfaktor
			$minfac=25.0;
			$maxfac=500.0;
			if($GLOBALS['pdf_array']['modus'] == 3)	// Gantt
				$maxfac=175.00;
			$disabled='';
			$fac=$GLOBALS['pdf_array']['fac'];
			if($GLOBALS['pdf_array']['fit'])
			{
				$disabled=' class="lightergrey"';
				$fac=1.0;
			}
			$inhalt .= '<td id="pdfdm_s0" nowrap'.$disabled.'><b>'.$lit.':</b></td>';
			$inhalt .= '<td id="pdfdm_s1" '.$disabled.'>'.fieldText('pdfs_fac',get_entry($fac*100, 'dezimal'),48,6,'right',false,$disabled).fieldHidden('pdfs_ofac',$GLOBALS['pdf_array']['ofac']*100).fieldHidden('pdfs_minfac',$minfac).fieldHidden('pdfs_maxfac',$maxfac).'</td>';
			$inhalt .= '<td id="pdfdm_s2"  nowrap'.$disabled.'>%&nbsp;&nbsp;<i>('.get_entry($minfac, 'dezimal').'-'.get_entry($maxfac, 'dezimal').')</i></td>';
			$inhalt .= '<td>&nbsp;</td>';
			$lit=meldung(1634);	// Querformat
			$inhalt .= '<td><b>'.$lit.':</b></td>';
			$inhalt .= '<td>'.fieldCheckbox('pdfs_lsfmt','',$GLOBALS['pdf_array']['lsfmt']).'</td>';
			$lit=meldung(2820);	// In Graustufen
			$inhalt .= '<td><b>'.$lit.':</b></td>';
			$inhalt .= '<td>'.fieldCheckbox('pdfs_grey','',$GLOBALS['pdf_array']['grey']).'</td>';
			$inhalt .= '</tr>';

			// ...Autom. Anpassung, Legende + Anhang
			$inhalt .= '<tr>';
			$lit=meldung(1260);	// Automatisch
			$disabled='';
			$dis=false;
			if($GLOBALS['pdf_array']['fitfix'])
			{
				$disabled=' class="lightergrey"';
				$dis=true;
			}
			$inhalt .= '<td id="pdfdm_a0" nowrap'.$disabled.'><b>'.$lit.':</b></td>';
			$inhalt .= '<td id="pdfdm_a1"'.$disabled.'>'.fieldCheckbox('pdfs_fit','',$GLOBALS['pdf_array']['fit'],false,$dis,$dis).'</td>';
			$inhalt .= '<td id="pdfdm_a2" nowrap'.$disabled.'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>('.meldung(1627).')</i></td>';	// (Seitenbreite)
			$inhalt .= '<td>&nbsp;</td>';
			if(isset($GLOBALS['pdf_array']['leg']))
			{
				$lit=meldung(1994);	// Legende
				$inhalt .= '<td><b>'.$lit.'</b></td>';
				$inhalt .= '<td>'.fieldCheckbox('pdfs_leg','',$GLOBALS['pdf_array']['leg']).fieldHidden('pdfs_leg_exists',1).'</td>';
			}
			else
				$inhalt .= '<td colspan="2"></td>';
			if(isset($GLOBALS['pdf_array']['att']))
			{
				$lit=meldung(1588);	// Anhang
				$inhalt .= '<td><b>'.$lit.':</b></td>';
				$inhalt .= '<td>'.fieldCheckbox('pdfs_att','',$GLOBALS['pdf_array']['att']).fieldHidden('pdfs_att_exists',1).'</td>';
			}
			else
				$inhalt .= '<td colspan="2"></td>';

			// Button
			$inhalt .= '<tr>';
			$lit=meldung(1635);	// PDF-Datei neu erstellen
			$pccnt=0;
			if($_GET['pccnt'])
				$pccnt=(int)$_GET['pccnt'];
			$tdadd='';
			if($_GET['pdfs_button'])
			{
				$pccnt++;
				$tdadd .= '<div style="float:left;margin-top:5px;color:#006b9f;"><b>'.meldung(3359).' ('.$pccnt.'x)</b></div>';	// The PDF-File has been recreated
			}
			$href=' href="'.urlParameter($_GET).'&pccnt='.$pccnt.'"';
//function fieldSubmit($fieldname, $value, $type='sub', $width=0, $dis=false, $nva=false, $force=false, $ltr='', $gparam='', $jsfunc='', $style='', $form='')
			$inhalt .= '<td class="maskb" colspan="8"><br />'.$tdadd.fieldSubmit('pdfs_button',$lit,'sub',330,false,false,false,'','',$href,'float:right;').'</td>';
			$inhalt .= '</tr>';

			$inhalt .= '</table>';
			$inhalt .= '</div>';
			$inhalt .= '</td>';
			$inhalt .= '</tr>';

			// Save selects
			fxSession('s','s');
		}

		$inhalt .= '<tr>';

		// PDF-Datei downloaden
//		$inhalt .= '<td width="'.$width.'" nowrap align="center" '.fxf_jsFunction('fileManager', 1, 'pdf', $fn, $ad).'>';
		$inhalt .= '<td width="'.$width.'" nowrap align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=1&type=pdf&filename='.$fn.'&dat='.$ad, $fn.'.pdf');
		$inhalt .= '<div class="fmaction">'.meldung(1576,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_dm.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		// PDF-Datei im Browser öffnen
//		$inhalt .= '<td width="'.$width.'" align="center" '.fxf_jsFunction('fileManager', 2, 'pdf', $fn, $ad).'>';
		$inhalt .= '<td width="'.$width.'" align="center">';
		$inhalt .= fxpALink('loader.php?url=filemanager.inc&pa=2&type=pdf&filename='.$fn.'&dat='.$ad);
		$inhalt .= '<div class="fmaction">'.meldung(1577,false).'<br /><img src="'.$GLOBALS['gfxpath'].'im_bw.png" border="0"></div>';
		$inhalt .= '</a>';
		$inhalt .= '</td>';

		$inhalt .= '</tr>';

		$inhalt .= '</table>';
		$inhalt .= '</td>';

		$inhalt=str_replace('{dh}',$dh,$inhalt);
		$showempty=false;
	}

	if($showempty)
		$dinhalt=popupMessage(meldung(1584,true), 'red');	// Ausgabe-File konnte nicht erzeugt werden!
	else
		$dinhalt=popupMask($title, '', '<table width="100%" height="100%" border="0" cellpadding="0" cellspacing="0">'.$inhalt.'</table>','',-1,-1);
	echo($dinhalt);
	$GLOBALS['_form']=$backupform;
}

/**
 * ???FUNCTION???
 *
 * @param various $sql    - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $replid - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function parse_report($sql, $replid=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($replid)
		$rvtr=array('$_spid'=>$GLOBALS['fxpglobals']['lang'], '$_psid'=>$GLOBALS['fxpglobals']['person'], '$_prid'=>(int)$GLOBALS['fxpglobals']['project'], '$_spracheid'=>$GLOBALS['fxpglobals']['lang'], '$_lang'=>$GLOBALS['fxpglobals']['lang'], '$_personenid'=>$GLOBALS['fxpglobals']['person'], '$_personen'=>$GLOBALS['fxpglobals']['person'], '$_projektid'=>(int)$GLOBALS['fxpglobals']['project'], '$_project'=>(int)$GLOBALS['fxpglobals']['project'], '&quot;'=>"'", "\r\n"=>' ', "\r"=>' ', "\n"=>' ', "\t"=>' ');
	else
		$rvtr=array('&quot;'=>"'", "\r\n"=>' ', "\r"=>' ', "\n"=>' ', "\t"=>' ');
	$sql=strtr(trim((string)$sql), $rvtr);
	$rarr['org']=$sql;

	$rvsql=strtolower((string)$sql);
	$rarr['low']=$rvsql;
	$sp=strpos($rvsql, ' ');
	$rvsql=substr((string)$rvsql, $sp+1);
	$ep=strpos($rvsql, ' from ');
	$rvsql=substr((string)$rvsql, 0, $ep);

	$rvtr=array('*'=>'', '#'=>'', '!'=>'');
	$rarr['sql']=strtr(substr((string)$sql, $sp+1+$ep), $rvtr);

	$sf=explode(',', $rvsql);
	if(is_array($sf))
	{
		foreach($sf as $sn => $fn)
		{
			$fn=trim((string)str_replace('distinct ', '', $fn));
			$summe		= false;
			$nowrap		= false;
			$special	= false;
			$sp=0;
			for($i=0; $i<strlen((string)$fn); $i++)
			{
				$c=substr((string)$fn, $i, 1);
				if($c == '*')
					$special=true;
				else if($c == '#')
					$summe=true;
				else if($c == '!')
					$nowrap=true;
				else
				{
					$fn=substr((string)$fn, $i);
					break;
				}
			}
			$dotpos=strpos($fn, '.');
			if($dotpos !== false)
				$fn=substr((string)$fn, $dotpos+1);
			$aliaspos1=strpos($fn, '|');
			$aliaspos2=strpos($fn, ' as ');
			if($aliaspos1 !== false)
			{
				$alias=trim(substr((string)$fn, $aliaspos1+1));
				$fn=trim(substr((string)$fn, 0, $aliaspos1));
			}
			else if($aliaspos2 !== false)
			{
				$alias=trim(substr((string)$fn, $aliaspos2+4));
				$fn=trim(substr((string)$fn, 0, $aliaspos2));
			}
			else
				$alias=$fn;

			$feldsql="SELECT id, feldtyp, feldname FROM felder WHERE ".db_like('feldname', $alias)." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
			$ida=db_values($feldsql);
			if(is_array($ida))
			{
				$fid	= (int)$ida['id'];
				$ftyp	= (int)$ida['feldtyp'];
				$fname	= $ida['feldname'];
			}
			else
			{
				$fid	= 0;
				$ftyp	= 1;
				$fname	= $alias;
			}
			$rarr['fld'][$sn]=array('id'=>$fid, 'fn'=>$fn, 'alias'=>$alias, 'typ'=>$ftyp, 'rn'=>$fname, 'special'=>$special, 'summe'=>$summe, 'nowrap'=>$nowrap);
		}
	}

	$sfl='';
	if(is_array($rarr['fld']))
	{
		foreach($rarr['fld'] as $sf)
			$sfl .= ', '.$sf['fn'];
		$rarr['sql']='SELECT'.substr((string)$sfl, 1).$rarr['sql'];
	}
	else
		$rarr['sql']='SELECT *'.$rarr['sql'];

	return $rarr;
}

/**
 * ???FUNCTION???
 */
function mg_show_report()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $_mgmsknr, $_mgmskname_real, $_mgmskname, $nl_b;

	$_mgtabname_real=$_mgmskname_real;
	if(substr((string)$_mgtabname_real, 0, 1) == '_')
		$_mgtabname_real=substr((string)$_mgtabname_real, 1);

	$GLOBALS['_fia'][1]['maske']	= false;
	$GLOBALS['_fia'][1]['form']	= false;

	if(!$_mgmsknr || !$_mgmskname_real || !$_mgmskname)
		return;

	// Hauptspalten (keine Zeilenumbruch dort)
	$namecolumn		= $_mgmskname.'_Name';
	$lnamecolumn	= strtolower((string)$namecolumn);
	$lidcolumn		= strtolower((string)$_mgmskname.'_ID');

	$GLOBALS[$namecolumn.'_nw']=true;

	// Standardstringreplace
	$mic_replace=array("'" => '´', "\\" => '\\\\');

	// Maskentitel (Report)
	$sql="SELECT text_kurz FROM texte WHERE id_referenz=".$_mgmsknr." AND id_ref_art=2 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$msktitel=meldung(269, false).'&nbsp;-&nbsp;'.db_value($sql);

	// Tabellen-ID
	$sql="SELECT id_tabellen FROM tabellen WHERE LOWER(tabname)='".strtolower((string)$_mgtabname_real)."'";
	$tid=(int)db_value($sql);

	$nmsknr=1000000000 - $_mgmsknr;
	$fld_arr=array();

	// Werte holen
	$validcolumn=false;
	$ordercolumn='';
	$ordersort=' ASC';
	if(isset($_POST['sort_post']) && strlen((string)$_POST['sort_post']))
	{
		$ordercolumn=substr((string)$_POST['sort_post'], 1);
		if(substr((string)$_POST['sort_post'], 0, 1) == 'd')
			$ordersort=' DESC';
	}
	else if(isset($_POST['sort_asc']))
		$ordercolumn=$_POST['sort_asc'];
	if(strlen((string)$ordercolumn) && $tid)
	{
		$sql="SELECT id FROM felder WHERE LOWER(feldname)='".strtolower((string)$ordercolumn)."' AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		$cid=(int)db_value($sql);
		if($cid)
		{
			$sql="SELECT idinfelder FROM tab_felder WHERE idinfelder=".$cid." AND idintabellen=".$tid;
			$fid=(int)db_value($sql);
			if($fid == $cid)
				$validcolumn=true;
		}
	}
	if(!$validcolumn)
		$ordercolumn=$namecolumn;
	$desc=true;
	if(substr((string)$ordersort, 1, 1) == 'A')
	{
		$_POST['sort_post']='u'.$ordercolumn;
		$desc=false;
	}
	else
		$_POST['sort_post']='d'.$ordercolumn;

	$lordercolumn=strtolower((string)$ordercolumn);
	$textcolumn=false;
	if(!isset($GLOBALS['fxpglobals']['dbparam']['float'][$lordercolumn]) && !isset($GLOBALS['fxpglobals']['dbparam']['integer'][$lordercolumn]))
		$textcolumn=true;

//	if(!$validcolumn || $textcolumn)
//		$ordercolumn=" LOWER(".$ordercolumn.")";

	$sql="SELECT * FROM ".$_mgtabname_real." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY ".$ordercolumn.$ordersort;
	$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]=db_values($sql, '*');

	// Include user rep file
	$resort=false;
	$inc='_fg_'.strtolower((string)$_mgmskname).'_'.$GLOBALS['fxpglobals']['client'].'_rep_user.inc';
	if(file_exists($inc))
		require($inc);

	$anz=0;
	if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
		$anz=sizeof($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]);

	// Neu sortieren
	if($anz && $resort && $validcolumn)
		SortSubArray($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']], $lordercolumn, $desc);

	// Aktionsicons integrieren
	// ...dazu Aktionsrechte ermitteln
	$allowed_change=false;
	$allowed_delete=false;
	$arights=tr_rechte($GLOBALS['fxpglobals']['tr']);
	if($arights & get_bit(FXP_CHANGE))
		$allowed_change=true;
	if($arights & get_bit(FXP_DELETE))
		$allowed_delete=true;
	// ...und Aktionsliterale ermitteln
	$sql="SELECT tabwert FROM wertetab WHERE wertetabid IN (55,56,57,58) AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position";
	$lita=db_values($sql,'*');
	$trlit=array(1=>$lita[0]['tabwert'], $lita[1]['tabwert'], $lita[2]['tabwert'], $lita[3]['tabwert']);
	// ...und Icons setzen
	if($anz)
	{

		foreach($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']] as $line => $data)
		{
			$aicons ='<img src="'.$GLOBALS['gfxpath'].'icsh.png" align="top" style="cursor:pointer;" '.fxf_jsFunction('actionColumn', FXP_DISPLAY, $data[$lidcolumn]).' tooltip="'.$trlit[FXP_DISPLAY].'">';
			if($allowed_change)
				$aicons .= '<img src="'.$GLOBALS['gfxpath'].'icch.png" align="top" style="cursor:pointer;margin-left:2px;" '.fxf_jsFunction('actionColumn', FXP_CHANGE, $data[$lidcolumn]).' tooltip="'.$trlit[FXP_CHANGE].'">';
			if($allowed_delete)
				$aicons .= '<img src="'.$GLOBALS['gfxpath'].'icde.png" align="top" style="cursor:pointer;margin-left:2px;" '.fxf_jsFunction('actionColumn', FXP_DELETE, $data[$lidcolumn]).' tooltip="'.$trlit[FXP_DELETE].'">';

			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$line][$lnamecolumn]=$aicons.'&nbsp;&nbsp;'.$data[$lnamecolumn];
		}
	}

	// Datei aufbereiten
	$mf='<'.'?'.'php'.$nl_b;

	// Maske
	$reptitle .= '&nbsp;&nbsp;<font class="s2 grey normal">('.meldung(1330, false, $anz).')</font>';	// Es wurden ... Einträge gefunden

	$mf .= '$GLOBALS[\'tmask\']=array'.$nl_b.'('.$nl_b;
	$mf .= '	\'breite\' => \'\','.$nl_b;
	$mf .= '	\'listenansicht\' => \'1\','.$nl_b;
	$mf .= '	\'maskenart\' => \'5\','.$nl_b;	// 5=Report Gesamt
	$mf .= '	\'border\' => \'\','.$nl_b;
	$mf .= '	\'status_maske\' => \'5\','.$nl_b;	// 5=PGM OK-Produktion
	$mf .= '	\'text_kurz\' => \''.strtr($msktitel, $mic_replace).$reptitle.'\','.$nl_b;
	$mf .= '	\'text_varlang\' => \''.strtr($_mgmskname_real, $mic_replace).'\','.$nl_b;
	$mf .= '	\'literal_20\' => \''.strtr($_mgmskname, $mic_replace).'\''.$nl_b.');'.$nl_b.$nl_b;

	if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']]))
	{
		$anz_zeilen		= 0;
		$anz_spalten	= 0;
		$zeilenindex	= array();
		$spaltenindex	= array();

		$s=1;
		$z=0;

		$mf .= '$GLOBALS[\'tmfields\']=array'.$nl_b.'('.$nl_b;

		// Alle Spalten durchgehen
		$msknameid=strtolower((string)$_mgmskname).'_id';
		foreach($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][0] as $feldname => $feldwert)
		{
//echo('<b>Feldname:</b> '.$feldname.'<br />');
			if(($feldname == 'mandanten_id') || ($feldname == $msknameid) || ($feldname == 'transid') || ($feldname == 'aenderungs_id') || ($feldname == 'zeitstempel'))
				continue;

			$feldid=db_value("SELECT id FROM felder WHERE ".db_like('feldname', $feldname));
			if(!is_array($fld_arr[$feldid]))
			{
				$mskfldsp="mandanten_id, id, feldname, alias_zu, feldtyp, laenge, nachkomma, html_grafik, texte_leer, nachschlage, nachschlage_art, kosten";
				$msktxtsp="id_text, text_kurz, text_varlang, literal_5, literal_10, literal_15, literal_20";

				$fld=db_values("SELECT ".$mskfldsp." FROM felder WHERE id=".$feldid);
				$alias_zu=$fld['alias_zu'];
				if($alias_zu > 0) // Alias-Feld gefunden
				{
					$texte_leer	= $fld['texte_leer'];
					$feldname	= $fld['feldname'];

					// Texte einladen, je nachdem ob Alias oder nicht
					if($texte_leer != 0)
						$idr=$alias_zu;
					else
						$idr=$feldid;
					$txt=db_values("SELECT ".$msktxtsp." FROM texte WHERE id_referenz=".$idr." AND id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang']);

					// Felder neu einladen, wenn Alias
					$fld=db_values("SELECT ".$mskfldsp." FROM felder WHERE id=".$alias_zu);
					$fld['feldname']=$feldname;
				}
				else // Kein Alias-Feld, Texte einladen
					$txt=db_values("SELECT ".$msktxtsp." FROM texte WHERE id_referenz=".$feldid." AND id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang']);

				$fld_arr[$feldid]=$fld;
				if(is_array($txt))
				{
					foreach($txt as $k => $v)
						$fld_arr[$feldid][$k]=$v;
				}
			}

			$anzgr1='3';
			$horausr='left';
			$sanz='0';
			$ftp=$fld_arr[$feldid]['feldtyp'];
//fxDebug($fld_arr[$feldid], $feldname);
			if(!$fld_arr[$feldid]['nachschlage'])
			{
				$rvalue=check_replacevalue($feldname);
				if(!$rvalue)
				{
					if(($ftp == 5) || ($ftp == 6) || ($ftp == 9) || ($ftp == 11) || ($ftp == 12) || ($ftp == 14))
					{
						$horausr='right';
						$anzgr1='2';
					}
					else if(($ftp == 4) || ($ftp == 7) || ($ftp == 10))
						$horausr='center';
				}
			}
			if(($ftp == 1) || ($ftp == 2) || ($ftp == 15))
			{
				$anzgr1='4';
				$sanz='-100';
			}
			else if($ftp == 4)
				$anzgr1='1';

			$sql="SELECT anzeigeart FROM maskenfelder WHERE eingabefeld=".$feldid." AND anzeigeart>4 AND nummer=".$_mgmsknr;
			$anzart=(int)db_value($sql);
			if(!$anzart)
			{
				$anzart=5;
				if(($ftp == 6) || ($ftp == 11))
					$anzart=14;
			}

			// Zeile schreiben
			$mf .= '	'.$z.' => array'.$nl_b.'	('.$nl_b;
			$mf .= '		\'nummer\' => \''.$nmsknr.'\','.$nl_b;
			$mf .= '		\'zeile\' => \'1\','.$nl_b;
			$mf .= '		\'spalte\' => \''.$s.'\','.$nl_b;
			$mf .= '		\'anzeigeart\' => \''.$anzart.'\','.$nl_b;
			$mf .= '		\'eingabefeld\' => \''.$feldid.'\','.$nl_b;
			$mf .= '		\'anzeigegroesse1\' => \''.$anzgr1.'\','.$nl_b;
			$mf .= '		\'anzeigegroesse2\' => \'0\','.$nl_b;
			$mf .= '		\'horausr\' => \''.$horausr.'\','.$nl_b;
			$mf .= '		\'vertausr\' => \'top\','.$nl_b;
			$mf .= '		\'spaltenanzahl\' => \''.$sanz.'\','.$nl_b;
			$mf .= '		\'zeilenanzahl\' => \'0\','.$nl_b;
			$mf .= '		\'spaltenbreite\' => \'\','.$nl_b;
			$mf .= '		\'zeilenhoehe\' => \'\','.$nl_b;
			$mf .= '		\'literal_overwrite\' => \'\','.$nl_b;
			$mf .= '		\'literal_value\' => \''.strtr($fld_arr[$feldid]['literal_'.((int)$anzgr1*5)], $mic_replace).'\','.$nl_b;
			$mf .= '		\'defaultwert\' => \'\','.$nl_b;
			$mf .= '		\'spezial\' => \'\','.$nl_b;
			$mf .= '		\'idintab\' => \'0\','.$nl_b;
			$mf .= '		\'zusatzparameter_int1\' => \'\','.$nl_b;
			$mf .= '		\'zusatzparameter_int2\' => \'\','.$nl_b;
			$mf .= '		\'zusatzparameter_txt\' => \'\','.$nl_b;
			foreach($fld_arr[$feldid] as $sm_lit => $sm_val)
				$mf .= '		\''.$sm_lit.'\' => \''.strtr($sm_val, $mic_replace).'\','.$nl_b;
			$mf .= '	),'.$nl_b;
			$sanz='';
			$z++;

			// Zeilen prüfen
			for($zsc=0; $zsc<$anz_zeilen; $zsc++)
			{
				if($zeilenindex[$zsc] == 1)
					$zsc=$anz_zeilen+1;
			}
			if($zsc == $anz_zeilen)
				$zeilenindex[$anz_zeilen++]=1;

			// Spalten prüfen
			for($zsc=0; $zsc<$anz_spalten; $zsc++)
			{
				if($spaltenindex[$zsc] == $s)
					$zsc=$anz_spalten+1;
			}
			if($zsc == $anz_spalten)
				$spaltenindex[$anz_spalten++]=$s;

			$s++;
		}
		$mf .= ');'.$nl_b.$nl_b;

		// Zeilen und Spaltenindizes sortieren, wenn existent
		if(fxIsArray($zeilenindex))
			sort($zeilenindex);
		if(fxIsArray($spaltenindex))
			sort($spaltenindex);

		$mfa['rows']	= $zeilenindex;
		$mfa['columns']	= $spaltenindex;

		// alle Felder vorab auf 'blank' setzen
		for($zanz=0; $zanz<$anz_zeilen; $zanz++)
		{
			$mfa['height'][$zeilenindex[$zanz]]=0;
			for($sanz=0; $sanz<$anz_spalten; $sanz++)
			{
				if($zanz == 0)
					$mfa['width'][$spaltenindex[$sanz]]=0;
				$mfa[$zeilenindex[$zanz]][$spaltenindex[$sanz]]='blank';
			}
		}

		if(fxIsArray($mfa))
		{
			$mf .= '$GLOBALS[\'tmfieldsarray\']=array'.$nl_b.'('.$nl_b;
			foreach($mfa as $sm_nr => $sm_arr)
			{
				$mf .= '	\''.$sm_nr.'\' => array'.$nl_b.'	('.$nl_b;
				if(is_array($sm_arr))
				{
					foreach($sm_arr as $sm_lit => $sm_val)
						$mf .= '		\''.$sm_lit.'\' => \''.strtr($sm_val, $mic_replace).'\','.$nl_b;
				}
				$mf .= '	),'.$nl_b;
			}
			$mf .= ');'.$nl_b;
		}
		else
			$mf .= '$GLOBALS[\'tmfieldsarray\']=array();'.$nl_b;
	}
	else
	{
		$mf .= '$GLOBALS[\'tmfields\']=array();'.$nl_b.$nl_b;
		$mf .= '$GLOBALS[\'tmfieldsarray\']=array();'.$nl_b;
	}
	$mf .= '?>';

	// Masken-Datei speichern
	$mn=$GLOBALS['micpath'].'m'.substr('0000000000'.$nmsknr, -10).'_'.$GLOBALS['fxpglobals']['lang'].'.inc';
	$se=fxSave($mn, $mf);
	if(!$se)	// Save successful -> Display mask
		echo(maske($nmsknr));
}

/**
 * Sortierfeld füllen
 *
 * @param various $arr - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_sortierfeld($arr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!fxIsArray($arr))
		return array('array'=>false, 'anzahl'=>0, 'select'=>'~|');

	$rc=sizeof($arr);
	$sla[-1]='- '.meldung(423, false);	// ausblenden
	$sla[0]='+ '.meldung(424, false);	// einblenden
	$wt_pos	= meldung(425, false);		// Pos.
	$sta='';
	for($sc=0; $sc<$rc; $sc++)
		$sla[1+$sc]='= '.$wt_pos.' '.substr('0'.(1+$sc), -2);
	// Sortierreihenfolge festlegen - 3. Durchgänge
	$einbl=0;
	for($sd=0; $sd<3; $sd++)
	{
		foreach($arr as $feldname => $f)
		{
			$fn=$feldname;
			if(substr((string)$fn, 1, 1) == '.')
				$fn=substr((string)$fn, 2);
			// Selectwert ermitteln...
			if(isset($GLOBALS['sf_'.$fn]))
				$selv=(int)$GLOBALS['sf_'.$fn];
			else if(isset($f['def']))
				$selv=(int)$f['def'];
			else
				$selv=0;

			if(!$sd)			// Pos. x
			{
				if($selv <= 0)
					continue;
				$sp=$selv;
				$einbl++;
			}
			else if($sd == 1)	// einblenden
			{
				if($selv != 0)
					continue;
				$sp=1;
				$einbl++;
			}
			else				// ausblenden
			{
				if($selv >= 0)
					continue;
				$sp=99;
			}

			// Position ermitteln
			while($sta[$sp])
				$sp++;

			// Literal ermitteln
			if(!$f['lit'])
				$f['lit']='15';
			$sql ="SELECT";
			$sql .= " f.mandanten_id, f.id, f.feldname, f.alias_zu, f.feldtyp, f.laenge, f.nachkomma, f.html_grafik, f.texte_leer, f.nachschlage, f.nachschlage_art, f.kosten,";
			$sql .= " t.id_text, t.text_kurz, t.text_varlang, t.anmerkungintern AS anmerkung, t.literal_".$f['lit']." AS literal_5, t.literal_".$f['lit']." AS literal_10, t.literal_".$f['lit']." AS literal_15, t.literal_".$f['lit']." AS literal_20, t.literal_".$f['lit']." AS lit";
			$sql .= " FROM felder f, texte t";
			$sql .= " WHERE t.id_referenz=f.id AND ".db_like('f.feldname', $fn)." AND t.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND t.id_ref_art=3 AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
			$far=db_values($sql);
			$sfn=$far['lit'];
			if(!$sfn)
				$sfn='*'.$feldname;

			// Array füllen
			$sta[$sp]=$far;
			$sta[$sp]['feld']	= $feldname;
			$sta[$sp]['selv']	= $selv;
			$sta[$sp]['farray']	= $f;
		}
	}
	ksort($sta, SORT_NUMERIC);

	$rs='~<table border="0" width="100%" cellpadding="1" cellspacing="0">';
	$afc=0;
	$tro=false;
	foreach($sta as $st)
	{
		$selv	= $st['selv'];
		$f		= $st['farray'];

		$fn=$st['feld'];
		if(substr((string)$fn, 1, 1) == '.')
			$fn=substr((string)$fn, 2);

		// Zugehörige Grafik ermitteln
		if($selv < 0)
			$gfx='db_nnl';
		else if($selv > 0)
			$gfx='db_cst';
		else
			$gfx='db_idx';

		if(!($afc%2))
		{
			$rs .= '<tr>';
			$tro=true;
			$tw='20';
		}
		else
			$tw='40';
		$rs .= '<td width="'.$tw.'" align="right" nowrap><img src="'.$GLOBALS['gfxpath'].$gfx.'.png"></td>';
		$rs .= '<td width="130" nowrap><font class="grey">'.substr('0'.($afc+1), -2).'</font>&nbsp;<font class="lit">'.$st['lit'].':</font></td>';
		$GLOBALS['sf_'.$fn.'_ds_array']=$sla;
		$rs .= '<td nowrap>'.fieldSelect('sf_'.$fn,$selv,290).'</td>';
		if($afc%2)
		{
			$rs .= '</tr>';
			$tro=false;
		}

		$afc++;
	}
	if($tro)
	{
		$rs .= '<td colspan="3">&nbsp;</td>';
		$rs .= '</tr>';
		$tro=false;
	}
	$rs .= '</table>';

	return array('array'=>$sta, 'anzahl'=>$einbl, 'select'=>$rs);
}

/**
 * ???FUNCTION???
 *
 * @param various $arr - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function create_sortmask($arr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl_b;

	if(!is_array($arr) || !is_array($arr['array']))
		return '';

//fxDebug($arr, 'Sortierarray');

	// Maskennummer
	$nmsknr='9999';

	// Maskentitel
	$nmskttl=meldung(426, false);	// Datenblatt Report

	// Datei aufbereiten
	$mf ='<?php'.$nl_b;

	// Maske
	$mf .= '$GLOBALS[\'tmask\']=array'.$nl_b.'('.$nl_b;
	$mf .= '	\'breite\' => \'940\','.$nl_b;
	$mf .= '	\'listenansicht\' => \'1\','.$nl_b;
	$mf .= '	\'maskenart\' => \'5\','.$nl_b;	// 5=Report Gesamt
	$mf .= '	\'border\' => \'\','.$nl_b;
	$mf .= '	\'status_maske\' => \'5\','.$nl_b;	// 5=PGM OK-Produktion
	$mf .= '	\'text_kurz\' => \''.$nmskttl.'\','.$nl_b;
	$mf .= '	\'text_varlang\' => \''.$nmskttl.'\','.$nl_b;
	$mf .= '	\'literal_20\' => \''.$nmskttl.'\''.$nl_b.');'.$nl_b.$nl_b;

	// Maskenfelder
	$z=0;

	$columns	= '	\'columns\' => array'.$nl_b.'	('.$nl_b;
	$width		= '	\'width\' => array'.$nl_b.'	('.$nl_b;
	$line		= '	\'1\' => array'.$nl_b.'	('.$nl_b;

	$mf .= '$GLOBALS[\'tmfields\']=array'.$nl_b.'('.$nl_b;
	foreach($arr['array'] as $fa)
	{
		if($fa['selv'] < 0)	// ausgeblendet
			continue;

		$sa='0';
		if(!$z)
			$sa='-100';

		$mf .= '	'.$z.' => array'.$nl_b.'	('.$nl_b;
		$mf .= '		\'nummer\' => \''.$nmsknr.'\','.$nl_b;
		$mf .= '		\'maskenfeldid\' => \''.(1000+$z).'\','.$nl_b;
		$mf .= '		\'zeile\' => \'1\','.$nl_b;
		$mf .= '		\'spalte\' => \''.(1+$z).'\','.$nl_b;
		$mf .= '		\'anzeigeart\' => \'5\','.$nl_b;	// Nur-Lesen-Textfeld
		$mf .= '		\'eingabefeld\' => \''.$fa['id'].'\','.$nl_b;
		$mf .= '		\'anzeigegroesse1\' => \'0\','.$nl_b;
		$mf .= '		\'anzeigegroesse2\' => \'0\','.$nl_b;
		$mf .= '		\'muss\' => \'0\','.$nl_b;
		$mf .= '		\'horausr\' => \'\','.$nl_b;
		$mf .= '		\'vertausr\' => \'\','.$nl_b;
		$mf .= '		\'spaltenanzahl\' => \''.$sa.'\','.$nl_b;
		$mf .= '		\'zeilenanzahl\' => \'0\','.$nl_b;
		$mf .= '		\'spaltenbreite\' => \'\','.$nl_b;
		$mf .= '		\'zeilenhoehe\' => \'\','.$nl_b;
		$mf .= '		\'literal_overwrite\' => \'\','.$nl_b;
		$mf .= '		\'literal_value\' => \''.$fa['lit'].'\','.$nl_b;
		$mf .= '		\'defaultwert\' => \'\','.$nl_b;
		$mf .= '		\'idintab\' => \'\','.$nl_b;
		$mf .= '		\'zusatzparameter_int1\' => \'\','.$nl_b;
		$mf .= '		\'zusatzparameter_int2\' => \'\','.$nl_b;
		$mf .= '		\'zusatzparameter_text\' => \'\','.$nl_b;

		foreach($fa as $sm_lit => $sm_val)
		{
			if(($sm_lit == 'lit') || ($sm_lit == 'feld') || ($sm_lit == 'selv') || ($sm_lit == 'farray'))
				continue;
			$mf .= '		\''.$sm_lit.'\' => \''.str_replace("'", '´', $sm_val).'\','.$nl_b;
		}
		$mf .= '	),'.$nl_b;

		// Maskenfeld-Array
		$columns	.= '		\''.$z.'\' => \''.(1+$z).'\','.$nl_b;
		$width		.= '		\''.(1+$z).'\' => \'0\','.$nl_b;
		$line		.= '		\''.(1+$z).'\' => \'blank\','.$nl_b;

		$z++;
	}
	$mf .= ');'.$nl_b;

	// Maskenfeld-Array
	$columns	.= '		\''.$z.'\' => \'99\','.$nl_b.'	),'.$nl_b;
	$width		.= '		\'99\' => \'0\','.$nl_b.'	),'.$nl_b;
	$line		.= '		\'99\' => \'blank\','.$nl_b.'	),'.$nl_b;

	$mf .= '$GLOBALS[\'tmfieldsarray\']=array'.$nl_b.'('.$nl_b;
	$mf .= '	\'rows\' => array'.$nl_b.'	('.$nl_b.'		\'0\' => \'1\','.$nl_b.'	),'.$nl_b;
	$mf .= $columns;
	$mf .= '	\'height\' => array'.$nl_b.'	('.$nl_b.'		\'1\' => \'0\','.$nl_b.'	),'.$nl_b;
	$mf .= $width;
	$mf .= $line;
	$mf .= ');'.$nl_b;

	$mf .= '?>';

	// Masken-Datei speichern
	$mn=$GLOBALS['micpath'].'m'.substr('0000000000'.$nmsknr, -10).'_'.$GLOBALS['fxpglobals']['lang'].'.inc';
	fxSave($mn, $mf);

	// Maske erstellen
	if(file_exists($mn))
	{
		$GLOBALS['maske'.$nmsknr.'_sf']=true;
		$ms=maske($nmsknr);
	}
	else
		$ms='<b>ERROR</b> Could not write temporary report mask!<br />';

	return $ms;
}

/**
 * ???FUNCTION???
 *
 * @param various $hstr - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function print_hidden($hstr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen((string)$hstr))
		return '';

	$rstr='';
	$harr=explode('&', trim((string)$hstr));
	foreach($harr as $z)
	{
		$eqpos	= strpos($z, '=');
		$name	= substr((string)$z, 0, $eqpos);
		$value	= substr((string)$z, $eqpos+1);

		$rstr .= fieldHidden($name,$value);
	}

	return $rstr;
}

/**
 * ???FUNCTION???
 *
 * @param various $wertetabid - Mandatory parameter:               ???PARAMETER???
 * @param integer $grp        - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function check_mail($wertetabid, $grp=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rv=false;

	$grp=strtoupper((string)$grp);
	if((!$grp && $GLOBALS['fxpglobals']['settings']['ems'][$wertetabid]['ac']) || ($grp && $GLOBALS['fxpglobals']['settings']['ems'][$wertetabid][$grp]))
		$rv=true;

	return $rv;
}

/**
 * ???FUNCTION???
 *
 * @param various $data    - Mandatory parameter:                            ???PARAMETER???
 * @param string  $md      - Optional parameter (default = 'i'):             ???PARAMETER???
 * @param string  $tabname - Optional parameter (default = 'maplanung'):     ???PARAMETER???
 * @param string  $primkey - Optional parameter (default = 'maplanung_id'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function restore_sql($data, $md='i', $tabname='maplanung', $primkey='maplanung_id')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid		= $GLOBALS['fxpglobals']['client'];
	$md			= strtolower(substr((string)$md, 0, 1));
	$tabname	= strtolower((string)trim((string)$tabname));
	$primkey	= strtolower((string)trim((string)$primkey));

	if(!is_array($data) || !$md || !$tabname || !$primkey)
		return '';

	$sql1		= '';	// für INSERT
	$sql2		= '';
	$sql3		= '';	// für UPDATE

	foreach($data as $column => $entry)
	{
		if(($entry === NULL) || !strlen((string)$entry))
			$entry='NULL';
		else
		{
			if(!is_array($GLOBALS['rest_feldtyp']) || !isset($GLOBALS['rest_feldtyp'][$column]))
				$GLOBALS['rest_feldtyp'][$column]=(int)db_value("SELECT feldtyp FROM felder WHERE ".db_like('feldname', $column)." AND mandanten_id IN (0,".$mid.")");
			$ftyp=$GLOBALS['rest_feldtyp'][$column];
			 if(($ftyp == 1) || ($ftyp == 3) || ($ftyp == 7) || ($ftyp == 9) || ($ftyp == 10) || ($ftyp == 13) || ($ftyp == 15))
				$entry="'".$entry."'";
		}

		if($sql1)
			$sql1 .= ', ';
		$sql1 .= $column;

		if($sql2)
			$sql2 .= ', ';
		$sql2 .= $entry;

		if($sql3)
			$sql3 .= ', ';
		$sql3 .= $column.'='.$entry;
	}

	if($md == 'i')
		return "DELETE FROM ".$tabname." WHERE ".$primkey."=".$data[$primkey];
	else if($md == 'u')
		return "UPDATE ".$tabname." SET ".$sql3." WHERE ".$primkey."=".$data[$primkey];
	else if($md == 'd')
		return "INSERT INTO ".$tabname." (".$sql1.") VALUES (".$sql2.")";
	else
		return '';
}

/**
 * ???FUNCTION???
 *
 * @param integer $mid - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $lid - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $xml - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function appcategory($mid=0, $lid=0, $xml=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$mid)
		$mid=$GLOBALS['fxpglobals']['client'];
	if(!$lid)
		$lid=$GLOBALS['fxpglobals']['lang'];

	// Kategorie-Typen
	$tc[FXP_AC_PRIVATE]			= array('txt'=>'t523', 'tart'=>array(FXP_AT_PRIVATE));
	$tc[FXP_AC_BUSINESS]	= array('txt'=>'t522', 'tart'=>array(FXP_AT_MEETING, FXP_AT_CONFERENCE, FXP_AT_BUSINESSDINNER, FXP_AT_CONTACT, FXP_AT_MESSAGE, FXP_AT_TODO, FXP_AT_PROJECTMESSAGE));
	$tc[FXP_AC_ABSENT]			= array('txt'=>'t524', 'tart'=>array(FXP_AT_ABSENT, FXP_AT_VACATION, FXP_AT_VACATIONREQUEST, FXP_AT_SPECIALVACATION, FXP_AT_SPECIALVACATIONREQUEST, FXP_AT_SICK, FXP_AT_SICKCALL, FXP_AT_TRAINING, FXP_AT_TRAININGREQUEST, FXP_AT_SLIDINGDAY, FXP_AT_SLIDINGDAYREQUEST));
	$tc[FXP_AC_VARIOUS]			= array('txt'=>'w809', 'tart'=>array(FXP_AT_REMINDER, FXP_AT_INVITATION, FXP_AT_VARIOUS));

	$rar=array();
	foreach($tc as $tkid => $t)
	{
		// Kategorie-Text
		if(substr((string)$t['txt'], 0, 1) == 't')	// aus Texte
		{
			$tid=(int)substr((string)$t['txt'], 1);
			$txt=db_value("SELECT literal_15 FROM texte WHERE id_referenz=".$tid." AND id_ref_art=3 AND id_sprache=".$lid);
		}
		else
		{
			$tid=(int)substr((string)$t['txt'], 1);
			$txt=db_value("SELECT tabwert FROM wertetab WHERE wertetabid=".$tid." AND mandanten_id=0 AND id_sprache=".$lid);
		}
		$rar[$tkid]=array('name'=>$txt, 'tart'=>'', 'csv'=>'');
		foreach($t['tart'] as $ta)
		{
			$ta=(int)$ta;

			$rar[$tkid]['tart'][$ta]=array('bit'=>255, 'name'=>db_value("SELECT tabwert FROM wertetab WHERE wertetabid=".$ta." AND mandanten_id=0 AND id_sprache=".$lid));

			$perm=db_value("SELECT aktionscode FROM rechte_referenz WHERE rechte_ref=65 AND id_referenz=".$ta." AND mandanten_id=".$mid);
			if(!strlen((string)$perm))
				$perm=db_value("SELECT aktionscode FROM rechte_referenz WHERE rechte_ref=65 AND id_referenz=".$ta." AND mandanten_id=0");
			if(strlen((string)$perm))
				$rar[$tkid]['tart'][$ta]['bit']=(int)$perm;

			if($rar[$tkid]['csv'])
				$rar[$tkid]['csv'] .= ',';
			$rar[$tkid]['csv'] .= $ta;
		}
	}
	if(!$xml)
		return $rar;

	$xs='';
	if($xml && is_array($rar))
	{
		foreach($rar as $cid => $r)
		{
			$xs .= "<CATEGORY>\n";
			$xs .= "\t<ID>".$cid."</ID>\n";
			$xs .= "\t<NAME>".$r['name']."</NAME>\n";
			if(is_array($r['tart']))
			{
				foreach($r['tart'] as $atid => $a)
				{
					$xs .= "\t<APPOINTMENT_TYPE>\n";
					$xs .= "\t\t<ID>".$atid."</ID>\n";
					$xs .= "\t\t<NAME>".$a['name']."</NAME>\n";
					$xs .= "\t\t<PERMISSION>".$a['bit']."</PERMISSION>\n";
					$xs .= "\t</APPOINTMENT_TYPE>\n";
				}
			}
			$xs .= "</CATEGORY>\n";
		}
	}
	return $xs;
}

/**
 * Aufbau Array $data
 * -------------------------------------------------
 * $data	[0]		[0]		= Wert 0 für Graph 0
 * [1]		= Wert 1 für Graph 0
 * .
 * .
 * [y]		= Wert y für Graph 0
 * $data	[1]		[0]		= Wert 0 für Graph 1
 * [1]		= Wert 1 für Graph 1
 * .
 * .
 * [y]		= Wert y für Graph 1
 * .
 * .
 * $data	[x]		[0]		= Wert 0 für Graph x
 * [1]		= Wert 1 für Graph x
 * .
 * .
 * [y]		= Wert y für Graph x
 * $data	['lab']	[0]		= Y-Label 0
 * [1]		= Y-Label 1
 * .
 * .
 * [y]		= Y-Label y
 * $data	['col']	[0]		= Color für Graph 0
 * [1]		= Color für Graph 1
 * .
 * .
 * [x]		= Color für Graph x
 * $data	['leg']	[0]		= Legend für Graph 0
 * [1]		= Legend für Graph 1
 * .
 * .
 * [x]		= Legend für Graph x
 *
 * @param various $data      - Mandatory parameter:                     ???PARAMETER???
 * @param integer $ctyp      - Optional parameter (default = 0):        ???PARAMETER???
 * @param integer $cd_width  - Optional parameter (default = 472):      ???PARAMETER???
 * @param integer $cd_height - Optional parameter (default = 164):      ???PARAMETER???
 * @param integer $fontsize  - Optional parameter (default = 7):        ???PARAMETER???
 * @param string  $tt        - Optional parameter (default = ''):       ???PARAMETER???
 * @param string  $einh      - Optional parameter (default = ''):       ???PARAMETER???
 * @param string  $titel     - Optional parameter (default = ''):       ???PARAMETER???
 * @param array   $bgcol     - Optional parameter (default = array()):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function bar_chartdir($data, $ctyp=0, $cd_width=472, $cd_height=164, $fontsize=7, $tt='', $einh='', $titel='', $bgcol=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$bild='';

	if(!isset($GLOBALS['_ext_chartdir']) || !strlen((string)$GLOBALS['_ext_chartdir']) || !fxIsArray($data))
		return $bild;

	$cd_valuesize=0;
	foreach($data as $id => $d)
	{
		if(!is_integer($id))
			continue;
		foreach($d as $val)
			$cd_valuesize=MAX($cd_valuesize, strlen((string)(int)$val));
	}
	if(!$cd_valuesize)
		return $bild;
	if(strlen((string)$einh))
	{
		if($einh == '')
			$einh='Euro';
		$cd_valuesize += 1+strlen((string)$einh);
	}
	$cd_valuesize=MAX($cd_valuesize, 4);

	$cd_labelsize=0;
	if(fxIsArray($data['lab']))
	{
		foreach($data['lab'] as $d)
			$cd_labelsize=MAX($cd_labelsize, strlen((string)$d));
	}

	$cd_titelsize=8;
	if(strlen((string)$titel))
		$cd_titelsize=30;

	$cd_legendsize=0;
	if(fxIsArray($data['leg']))
		$cd_legendsize=$fontsize+7;

	$cd_bordersize	= 12;
	$cd_valuesize	= $cd_valuesize*$fontsize;
	$cd_labelsize	= (int)($cd_labelsize*$fontsize*0.50);

	$cd_bwidth=$cd_width-(2*$cd_bordersize)-$cd_valuesize;
	if($cd_bwidth < 50)
	{
		$cd_bwidth	= 50;
		$cd_width	= $cd_bwidth+(2*$cd_bordersize)+$cd_valuesize;
	}
	$cd_bheight=$cd_height-(2*$cd_bordersize)-$cd_legendsize-$cd_labelsize-$cd_titelsize;
	if($cd_bheight < 50)
	{
		$cd_bheight	= 50;
		$cd_height	= $cd_bheight+(2*$cd_bordersize)+$cd_legendsize+$cd_labelsize+$cd_titelsize;
	}

	$c=new XYChart($cd_width, $cd_height);
	$c->setBackground($c->linearGradientColor(0, 0, 0, 100, 0xe0e0e0, 0xffffff), 0xbbbbbb);
	$c->setRoundedFrame(0xfeffffff);
	$c->setDropShadow(0xfeaaaaaa);

	$c->setPlotArea($cd_bordersize+$cd_valuesize, $cd_bordersize+$cd_legendsize+$cd_titelsize, $cd_bwidth-$cd_bordersize, $cd_bheight, 0xfafafa, 0xfcfcfc, 0x444444, 0xc8c8c8, Transparent);

	// Titel
	if(strlen((string)$titel))
	{
		$titleObj=$c->addTitle($titel, "arialbd.ttf", 10, 0x000000);
		$titleObj->setBackground(0xdedede, 0x000000, glassEffect());
	}

	// Legende
	if($cd_legendsize)
	{
		$legendObj=$c->addLegend($cd_bordersize+$cd_valuesize, -5+$cd_titelsize, false, '', $fontsize+1);
		$legendObj->setBackground(Transparent);
	}

	// Zahlenformate
	$nkstl=$GLOBALS['fxpglobals']['settings']['nkstellen'];
	$tsdtz='.';
	$deztz=$GLOBALS['fxpglobals']['settings']['deztrennzeichen'];
	if($deztz == '.')
		$tsdtz=',';
	$c->setNumberFormat($tsdtz, $deztz, '-');

	// Einheit X-Achse
	if(strlen((string)$einh))
		$c->yAxis->setLabelFormat('{value} '.$einh);

	// Automatische Skalierung aus
    $c->yAxis->setAutoScale(0, 0);

	// Daten
	$lw=1.50;
	foreach($data as $id => $d)
	{
		if(!is_integer($id))
			continue;

		switch($ctyp)
		{
			case FXP_CT_AREA:
				$c->addAreaLayer($d, $data['col'][$id], $data['leg'][$id], 3);
			break;

			default:
				$lineLayerObj=$c->addLineLayer2();
				$lineLayerObj->setLineWidth($lw);
				$lw += 0.50;
//				$lineLayerObj->set3D();
				$lineLayerObj->addDataSet($d, $data['col'][$id], $data['leg'][$id]);
			break;
		}
	}

	// Beschriftung Y-Achse
	if($cd_labelsize)
	{
		$c->yAxis->setLabelStyle("arial.ttf", $fontsize, 0x000000);

		// Beschriftung X-Achse
		$labelStyleObj=$c->xAxis->setLabelStyle("arial.ttf", $fontsize, 0x000000);
		$labelStyleObj->setFontAngle(45);
		$c->xAxis->setLabels($data['lab']);
	}

	// und als PNG zwischenspeichern
	$img=$c->makeChart2(PNG);
	$fic=0;
	while($fic < 100)
	{
		$pname=$GLOBALS['tmppath'].'~'.$GLOBALS['datetime'].substr('00000'.$GLOBALS['fxpglobals']['person'],-6).substr('0'.$fic,-2).'c.png';
		if(!file_exists($pname))
			break;
		$fic++;
	}
	$fname=saveChartDirectorImage($img, $pname);
	if(strlen((string)$fname))
		$bild='<img src="'.$fname.'" border="0" width="'.$cd_width.'" height="'.$cd_height.'" title="'.$tt.'">';

	return $bild;
}

/**
 * ???FUNCTION???
 *
 * @param various $img      - Mandatory parameter:                   ???PARAMETER???
 * @param various $filename - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $cut      - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function saveChartDirectorImage($img, $filename)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	fxDelete($filename);

	if($img)
	{
		if(function_exists('fxsppSaveChartDirectorImage'))
			$se=fxsppSaveChartDirectorImage($img, $filename);
		else
			$se=fxSaveBin($filename, $img);
		if($se)	// Save error
			$filename='';	// Error: Could not save PNG image!
	}
	else
		$filename='';	// Error: Could not create PNG image!

	return $filename;
}

/**
 * ???FUNCTION???
 *
 * @param various $filename - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function read_ini($filename)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Sprache
	if($GLOBALS['fxpglobals']['lang'])
		$spr=$GLOBALS['fxpglobals']['lang'];
	else
		$spr=7;

	// Prüfen, ob Datei existiert
	if(!file_exists($filename))
	{
		if($spr == 1)	// Deutsch
			$ret_error='Die Konfigurations-Datei &quot;<b>'.$filename.'</b>&quot; wurde nicht gefunden!';
		else			// English
			$ret_error='Couldn´t find the configuration file &quot;<b>'.$filename.'</b>&quot;!';
		return $ret_error;
	}

	$ini='';
	// komplettes File zeilenweise in ein Array einlesen
	$inhalt=@file($filename);
	if(!is_array($inhalt))
	{
		if($spr == 1)	// Deutsch
			$ret_error='Die Konfigurations-Datei &quot;<b>'.$filename.'</b>&quot; kann nicht ge&ouml;ffnet werden!';
		else			// English
			$ret_error='Can\'t open the configuration file &quot;<b>'.$filename.'</b>&quot;!';
		return $ret_error;
	}

	// Array durchgehen
	foreach($inhalt as $i)
	{
		$zeile		= '';
		$delblank	= true;
		$ist		= 0;

		for($z=0; $z<strlen((string)$i); $z++)	// alle Blanks entfernen
		{
			$iz1=substr((string)$i, $z, 1);
			$iz2=substr((string)$i, $z+1, 1);
			if(($iz1 == '/') && ($iz2 == '/')) // Kommentare überspringen
				break;
			else if(ord($iz1) == 0x20)	// Blank
			{
				if($delblank == false)
					$zeile .= $iz1;
			}
			else if($iz1 == '=')		// nach Definitionszeichen '=' Blanks übernehmen
			{
				$zeile .= $iz1;
				$ist=1;
			}
			else if($iz1 == ',')		// Komma behandeln wie Zeilenumbruch
			{
				$zeile .= $iz1;
				$delblank=true;
				$ist=0;
			}
			else if(ord($iz1) > 0x20)
			{
				$zeile .= $iz1;
				if($ist == 1)
					$delblank=false;
			}
		}

		$zeile=trim((string)$zeile);
		if(!strlen((string)$zeile)) // Leere Zeilen überspringen
			continue;

		// Werte überprüfen
		$pos=strpos($zeile, '=');
		if($pos !== false)
		{
			$nam=strtolower((string)trim(substr((string)$zeile, 0, $pos)));
			$val=trim(substr((string)$zeile, $pos+1));
		}
		else
		{
			$nam=strtolower((string)$zeile);
			$val='';
		}
		$ini[$nam]=$val;
	}

	return $ini;
}

/**
 * Funktion zum Ermitteln der Felder einer Maske
 *
 * @param string  $mnr - Optional parameter (default = ''): ???PARAMETER???
 * @param various $mc  - Optional parameter (default = -1): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function repFieldInfo($mnr='', $mc=-1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$fa=array('colspan'=>0, 'firstfield'=>"", 'lastfield'=>"", 'fields'=>array());

	if(!strlen((string)$mnr))
		$mnr=$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim((string)$mnr));

	if($mc == -1)
		$mc=$GLOBALS['_maskcounter'];

	// Maskeninfo holen
	$merr=sessionMask($mnr);

	// Maske vorhanden, dann Felder ermitteln
	if(!$merr && fxIsArray($GLOBALS['fx_mfields']) && fxIsArray($GLOBALS['fx_mfields'][$mnr]))
	{
		$maskenart=0;
		if(fxIsArray($GLOBALS['fx_mask']) && fxIsArray($GLOBALS['fx_mask'][$mnr]))
			$maskenart=(int)$GLOBALS['fx_mask'][$mnr]['maskenart'];

		$minline=0;
		if($maskenart > 3)			// ab 4=Report
			$minline=1;
		else if ($maskenart > 1)	// ab 2=Liste
			$minline=2;

		if($minline)
		{
			$fc=0;
			foreach($GLOBALS['fx_mfields'][$mnr] as $m)
			{
				$line=(int)$m['zeile'];
				if(($line >= $minline) && ($line < 9000))
				{
					$feldname=$m['feldname'];
					if(!isset($GLOBALS[$feldname.'_nv']) || !$GLOBALS[$feldname.'_nv'])
					{
						$hfn='rc_'.$mnr.'_'.$feldname;
						if(isset($_POST[$hfn]) && ($_POST[$hfn]%2 == 0))
						{
							$GLOBALS[$feldname.'_nv']=true;
//echo('<b>Hide field:</b> '.$feldname.'<br />');
						}
					}
					if(!isset($GLOBALS[$feldname.'_nv']) || !$GLOBALS[$feldname.'_nv'])
					{
						if(!strlen((string)$fa['firstfield']))
							$fa['firstfield']=$feldname;
						$fa['lastfield']=$feldname;
						$fa['colspan']++;
						$fa['fields'][$feldname]=$fc;

						$fc++;
					}
				}
			}
		}
	}

	return $fa;
}

/**
 * Funktion zum Schreiben einer Leerzeile in einer Maskenliste
 *
 * @param string  $mnr         - Optional parameter (default = ''):    ???PARAMETER???
 * @param various $mc          - Optional parameter (default = -1):    ???PARAMETER???
 * @param integer $z           - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $colspan     - Optional parameter (default = false): ???PARAMETER???
 * @param boolean $transparent - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $force       - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function leerzeile($mnr='', $mc=-1, $z=0, $colspan=false, $transparent=true, $force=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen((string)$mnr))
		$mnr=$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim((string)$mnr));

	if($mc == -1)
		$mc=$GLOBALS['_maskcounter'];

	$z=(int)$z;

	// Spezial für Programmfunktion 57 "std_rep_datum"
	if($GLOBALS['fxpglobals']['tr'] == 57)
	{
		if($GLOBALS['ln'] && $GLOBALS['pset'] && ($GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$GLOBALS['ln']]['tage'] != '~|'))
		{
			$fa=array(
				'Tage', 'Zeit_ID', 'Datum',
				'Monatsname', 'Tagesanzahl',
				'Taetigkeit_Text', 'Stundencheck',
				'Arbeitstage', 'Urlaub', 'sonderurlaub', 'Schulungstage', 'Krankheitstage', 'abwesenheit', 'GleitzeitStunden', 'SOLLStunden', 'ISTStunden', 'Delta'
			);
			foreach($fa as $fd)
			{
				$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$GLOBALS['ln']][strtolower((string)$fd)]='~|';
				$GLOBALS[$fd.$GLOBALS['srd_mnr'].'_'.$GLOBALS['ln'].'_ffs'] = 1;
			}
			$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][$GLOBALS['ln']]['icon_aendern']='~|';
			$GLOBALS['icon_aendern'.$GLOBALS['srd_mnr'].'_'.$GLOBALS['ln'].'_ffs'] = 0;

			$GLOBALS['ln']++;
		}
		return;
	}

	// Spezial für Programmfunktion 100, 259 + 269 ("userprefs")
	if(($GLOBALS['fxpglobals']['tr'] == 100) || ($GLOBALS['fxpglobals']['tr'] == 259) || ($GLOBALS['fxpglobals']['tr'] == 269))
	{
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['loeschen']='~|';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['nv_text1']='~|';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['preference']='~|';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['einstellungswert']='~|';
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['mc']][$GLOBALS['z']]['icon']='~|';

		$GLOBALS['Loeschen__'.$GLOBALS['mc'].'_'.$GLOBALS['z'].'_ffs']=1;
		$GLOBALS['nv_text1__'.$GLOBALS['mc'].'_'.$GLOBALS['z'].'_ffs']=1;
		$GLOBALS['preference__'.$GLOBALS['mc'].'_'.$GLOBALS['z'].'_ffs']=1;
		$GLOBALS['Einstellungswert__'.$GLOBALS['mc'].'_'.$GLOBALS['z'].'_ffs']=1;

		$GLOBALS['z']++;

		return;
	}

	if($z || $force)
	{
		// Maskeninfo holen
		$merr=sessionMask($mnr);

		// Maske vorhanden, dann Felder ermitteln
		if(!$merr && fxIsArray($GLOBALS['fx_mfields']) && fxIsArray($GLOBALS['fx_mfields'][$mnr]))
		{
			$maskenart=0;
			if(fxIsArray($GLOBALS['fx_mask']) && fxIsArray($GLOBALS['fx_mask'][$mnr]))
				$maskenart=(int)$GLOBALS['fx_mask'][$mnr]['maskenart'];

			$minline=0;
			if($maskenart > 3)			// ab 4=Report
				$minline=1;
			else if ($maskenart > 1)	// ab 2=Liste
				$minline=2;

			if($minline)
			{
				$feldname='';
				$fcol='';
				$lcol='';
				$ccol=0;
				foreach($GLOBALS['fx_mfields'][$mnr] as $m)
				{
					$line=(int)$m['zeile'];
					if(($line >= $minline) && ($line < 9000))
					{
						$feldname=strtolower((string)$m['feldname']);
						if($colspan)
						{
							$ccol++;
							if(!strlen((string)$fcol))
								$fcol=$feldname;
							$lcol=$feldname;
						}
						else if($transparent)
							$GLOBALS['fxptdata']['lvalues'][$mc][$z][$feldname]='~|';
						else
							$GLOBALS['fxptdata']['lvalues'][$mc][$z][$feldname]='~&nbsp;';
					}
				}
				if($ccol > 0)
				{
					if($ccol == 1)
						$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol]='~|';
					else
					{
						$txt = '<img class="pp" src="'.$filename.'" border="4" align="right">';
						$col = '?';
						if($transparent)
							$col = '*';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z][$fcol] = '^col_'.$col.'" nowrap colspan="'.$ccol.'"><!--|~&nbsp;';
						$GLOBALS['fxptdata']['lvalues'][$mc][$z][$lcol] = '~-->&nbsp;';
					}
				}

				$GLOBALS['empty__'.$mc.'_'.$z.'_ffs']=-1;
//echo('<b class=blue>SET</b> $empty__'.$mc.'_'.$z.'_ffs &rarr; <b>'.$GLOBALS['empty__'.$mc.'_'.$z.'_ffs'].'</b><hr size=1 />');

				if(strlen((string)$feldname))
					$z++;
			}
		}
	}

	return $z;
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname   - Mandatory parameter:                          ???PARAMETER???
 * @param string  $literaltype - Optional parameter (default = 'literal_20'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_literal($fieldname, $literaltype='literal_20')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lit='???';

	if(strlen((string)$fieldname))
	{
		$sql="SELECT t.".$literaltype.", f.alias_zu FROM felder f, texte t WHERE t.id_referenz=f.id AND t.id_ref_art=3 AND t.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND ".db_like('f.feldname', $fieldname);
		$lar=db_values($sql);
//fxDebug($lar, $sql);
		if(is_array($lar))
		{
			if(strlen((string)$lar[$literaltype]))
				$lit=$lar[$literaltype];
			else if($lar['alias_zu'])
			{
				$sql="SELECT ".$literaltype." FROM texte WHERE id_referenz=".$lar['alias_zu']." AND id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
				$lit=db_value($sql);
//fxDebug($lit, $sql);
			}
		}
	}

	return $lit;
}

/**
 * Create a message div
 *
 * @param string  $msg     - Optional parameter (default = ''):    Message that should be displayed
 * @param string  $id      - Optional parameter (default = ''):    Optional id of the created div
 * @param string  $col     - Optional parameter (default = ''):    Hex color, e.g. 'e1001a' (this is the value it will be set to if empty)
 * @param boolean $display - Optional parameter (default = true):  Style: Display the div (true) or not (false)
 * @param boolean $ret     - Optional parameter (default = false): Return the div string (true) or display it (false)
 *
 * @return (Optional) created message div
 */
function fxp_warn($warnung='', $id='', $col='', $display=true, $ret=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$col=trim((string)$col);
	if(strlen((string)$col) != 6)
		$col='e1001a';

	$dd='block';
	if(!$display)
		$dd='none';

	if(strlen((string)$id))
		$id='id="'.$id.'" ';

	$msg='<div '.$id.'class="s4b" style="margin-bottom:16px;margin-right:16px;border:2px solid #'.$col.';padding:12px;border-radius:6px;background:rgba('.hexdec(substr((string)$col,0,2)).','.hexdec(substr((string)$col,2,2)).','.hexdec(substr((string)$col,4)).', 0.075);color:#'.$col.';box-shadow:4px 4px 8px rgba(0,0,0, 0.3) inset, 4px 4px 8px rgba(0,0,0, 0.2);display:'.$dd.';">'.$warnung.'</div>';

	if($ret)
		return($msg);
	echo($msg);
}

/**
 * ???FUNCTION???
 *
 * @param string  $note    - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $id      - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $display - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $ret     - Optional parameter (default = false): ???PARAMETER???
 */
function fxp_note($note='', $id='', $display=true, $ret=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dd='block';
	if(!$display)
		$dd='none';

	if(strlen((string)$id))
		$id='id="'.$id.'" ';

	$msg='<div '.$id.'class="s4 green" style="margin-bottom:16px;border:1px solid #009f6b;padding:4px;border-radius:6px;background:#f0fff4;box-shadow:4px 4px 8px rgba(0,0,0, 0.25) inset, 4px 4px 8px rgba(0,0,0, 0.10);display:'.$dd.';">'.$note.'</div>';

	if($ret)
		return($msg);
	echo($msg);
}

/**
 * Funktion zum Ermitteln der Colspans in einem Report
 *
 * @param string  $mnr - Optional parameter (default = ''): ???PARAMETER???
 * @param various $mc  - Optional parameter (default = -1): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function rep_colspan($mnr='', $mc=-1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen((string)$mnr))
		$mnr=$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim((string)$mnr));

	if($mc == -1)
		$mc=$GLOBALS['_maskcounter'];

	// Rückgabearray
	$retarr=array('colspan'=>0, 'firstcol'=>"", 'lastcol'=>"", 'blastcol'=>"");

	// Maskeninfo holen
	$merr=sessionMask($mnr);

	// Maske vorhanden, dann Felder ermitteln
	if(!$merr && fxIsArray($GLOBALS['fx_mfields']) && fxIsArray($GLOBALS['fx_mfields'][$mnr]))
	{
		$maskenart=0;
		if(fxIsArray($GLOBALS['fx_mask']) && fxIsArray($GLOBALS['fx_mask'][$mnr]))
			$maskenart=(int)$GLOBALS['fx_mask'][$mnr]['maskenart'];

		if($maskenart > 3)	// ab 4=Report
		{
			$feldname='';
			foreach($GLOBALS['fx_mfields'][$mnr] as $m)
			{
				$line=(int)$m['zeile'];
				if($line == 1)
				{
					$feldname_orig	= trim((string)$m['feldname']);
					if(!$GLOBALS[$feldname_orig.'_nv'])
					{
						$feldname_lower=strtolower((string)$feldname_orig);
						$retarr['colspan']++;
						if(!strlen((string)$retarr['firstcol']))
						{
							$retarr['firstcol']=$feldname_lower;
							$retarr['blastcol']=$feldname_lower;
						}
						else
							$retarr['blastcol']=$retarr['lastcol'];
						$retarr['lastcol']=$feldname_lower;
					}
				}
			}
		}
	}

	return $retarr;
}

/**
 * ???FUNCTION???
 *
 * @param various $zdata    - Mandatory parameter:                     ???PARAMETER???
 * @param integer $rechnid  - Optional parameter (default = 0):        ???PARAMETER???
 * @param array $Person     - Optional parameter (default = array()):  ???PARAMETER???
 * @param array $Projekte   - Optional parameter (default = array()):  ???PARAMETER???
 */
function special_timereport($zdata, $rechnid=0, $Person=array(), $Projekte=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$scsv="";
	if(fxIsArray($zdata))
	{
		if(($Person === NULL) || !fxIsArray($Person))
			$Person=array();
		if(($Projekte === NULL) || !fxIsArray($Projekte))
			$Projekte=array();

		// Rechnungsnummer ermitteln, falls Rechnungs-ID mitgegeben wurde
		$rechnid=(int)$rechnid;
		$rechnnr='';
		if($rechnid)
		{
			$sql="SELECT rechnn_f FROM rechnung WHERE rechnungs_id=".$rechnid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$rechnnr=db_value($sql);
//fxDebug($rechnnr, $sql);
		}

		// Rechnungspositionsdaten (hier: Mehrwertsteuer) ermitteln, falls abgrechnete Positionen
		$rechnps=array();
		foreach($zdata as $z)
		{
			$rid='';
			$pid='';
			if(isset($z['rechnungs_id']) && $z['rechnungs_id'] && isset($z['positions_id']) && $z['positions_id'])
			{
				$rid=(int)$z['rechnungs_id'];
				$pid=(int)$z['positions_id'];
			}
			else if(isset($z['rechnungs_id_rk']) && $z['rechnungs_id_rk'] && isset($z['positions_id_rk']) && $z['positions_id_rk'])
			{
				$rid=(int)$z['rechnungs_id_rk'];
				$pid=(int)$z['positions_id_rk'];
			}
			if(strlen((string)$rid) && strlen((string)$pid))
			{
				$id=$rid.'_'.$pid;
				if(!isset($rechnps[$id]))
				{
					$sql="SELECT mwst_satz FROM rechnungsposition WHERE rechnungs_id=".$rid." AND positions_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp=db_values($sql);
					if(fxIsArray($tmp))
					{
						$rechnps[$id]=array('mwst_satz'=>get_entry($tmp['mwst_satz'], 'dezimal'));
					}
				}
			}
//fxDebug($rechnps, $rechnnr);
		}

		// Personendaten + Projektdaten ermitteln
		if(!sizeof($Person) || !sizeof($Projekte))
		{
			$pers_arr	= array();
			$pers_csv	= '';
			$prjvn_arr	= array();
			$prjvn_csv	= '';
			$prj_arr	= array();
			$prj_csv	= '';

			foreach($zdata as $z)
			{
				$persid=(int)$z['personen_id'];
				if(!isset($pers_arr[$persid]))
				{
					$pers_arr[$persid]=true;
					if(strlen((string)$pers_csv))
						$pers_csv .= ",";
					$pers_csv .= $persid;
				}

				$prjid=(int)$z['projekt_id'];
				if(!isset($prj_arr[$prjid]))
				{
					$prj_arr[$prjid]=true;
					$prjvn_arr[$z['vorgangsnummer']]=true;
					if(strlen((string)$prj_csv))
						$prj_csv .= ",";
					$prj_csv .= $prjid;
				}

				$tvn=$z['vorgangsnummer'];
				while(true)
				{
					if(!isset($prjvn_arr[$tvn]))
					{
						$prjvn_arr[$tvn]=true;
						if(strlen((string)$prjvn_csv))
							$prjvn_csv .= " OR ";
						$prjvn_csv .= "vorgangsnummer='".$tvn."'";
					}

					$tvn=substr((string)$tvn, 0, -5);
					if(strlen((string)$tvn) < 4)
						break;
				}
			}

			// ...Personendaten
			if(!sizeof($Person) && strlen((string)$pers_csv))
			{
				$sql ="SELECT personen_id, pname, vorname, personen_initialen FROM personen";
				$sql .= " WHERE personen_id IN (".$pers_csv.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY pname, vorname";
				$Person=db_values($sql, '*');
//fxDebug($Person, $sql);
			}

			// ...Projektdaten
			if(!sizeof($Projekte) && strlen((string)$prj_csv))
			{
				$sql   = "SELECT projekt_id, vorgangsnummer, name_projekt, ref_nummer FROM projekte";
				$sql .= " WHERE projekt_id IN (".$prj_csv.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$par=db_values($sql, '*');
//fxDebug($par, $sql);
				if(fxIsArray($par))
				{
					foreach($par as $p)
					{
						$p['name_projekt']=strtr($p['name_projekt'], $GLOBALS['caption_strtr']);
						$Projekte[$p['vorgangsnummer']]=$p;
					}
					unset($par);
//fxDebug($Projekte, '$Projekte');
				}
				if(strlen((string)$prjvn_csv))
				{
					$sql ="SELECT projekt_id, vorgangsnummer, name_projekt, ref_nummer FROM projekte";
					$sql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (".$prjvn_csv.")";
					$par =db_values($sql, '*');
//fxDebug($par, $sql);
					if(is_array($par))
					{
						foreach($par as $p)
						{
							$p['name_projekt']=strtr($p['name_projekt'], $GLOBALS['caption_strtr']);
							$Projekte[$p['vorgangsnummer']]=$p;
						}
						unset($par);
//fxDebug($Projekte, '$Projekte');
					}
				}
			}
		}
		if(($Person === NULL) || !fxIsArray($Person) || ($Projekte === NULL) || !fxIsArray($Projekte))
			return;

//fxDebug($zdata, '$zdata');
//fxDebug($Person, '$Person');
//fxDebug($Projekte, '$Projekte');

		// Zeitraum ermitteln
		$von='99999999';
		$bis='00000000';
		foreach($zdata as $z)
		{
			if($z['ist_beginn'] !== NULL)
				$von=min($von, substr((string)$z['ist_beginn'], 0,8));
			if($z['ist_ende'] !== NULL)
				$bis=max($bis, substr((string)$z['ist_ende'], 0,8));
		}

		// Spezialreport generieren
		$weekdays=weekdays($von, $bis);
		if(!isset($GLOBALS['wochentagsnamen']))
			$GLOBALS['wochentagsnamen']=fxf_getWeekdayNames(2);

		// Land-Array
		$rk_land=array();

		// Reisekosten-Arrays
		$rk_art=fxf_getReferenceValues(1884);
		$rk_kat=fxf_getReferenceValues(1291);
		// Evtl. selbstdefinierten Begriff statt "Tagesangaben" (ID=1980) verwenden
		if(($GLOBALS['fxpglobals']['settings']['txt_tangaben'] !== NULL) && strlen((string)$GLOBALS['fxpglobals']['settings']['txt_tangaben']))
			$rk_kat[1980]=trim((string)$GLOBALS['fxpglobals']['settings']['txt_tangaben']);
		$rk_kat[1980]='['.$rk_kat[1980].']';

		// Nachkommastellen
		$cnks=(int)$GLOBALS['fxpglobals']['settings']['nkstellen']+1;
		$mwst=get_entry(get_double($GLOBALS['fxpglobals']['settings']['mwst_satz']), 'dezimal');
		$dez0=get_entry(0.0, 'dezimal');
		$dez1=get_entry(0.0, 'dezimal', $cnks);

		$carr =	array(
					'+Rechnungs-Nr.'=>"+rechnnr", 'Name'=>"+scsv_name", 'Mitarbeiter-Identifikations-Nr.'=>"+scsv_ident", 'Datum'=>"scsv_datum", 'Wochentag'=>"+scsv_wochentag",
					'Zeit von'=>"scsv_zeitvon", 'Zeit bis'=>"scsv_zeitbis", 'Pause'=>"scsv_pause", '1. Vorgangsebene'=>"+scsv_vorgang1", '2. Vorgangsebene'=>"+scsv_vorgang2",
					'3. Vorgangsebene'=>"+scsv_vorgang3", '4. Vorgangsebene'=>"+scsv_vorgang4", '5. Vorgangsebene'=>"+scsv_vorgang5", 'Aufgabe'=>"+scsv_aufgabe",
					'Referenznummer'=>"+scsv_referenznummer", 'Taetigkeit'=>"+scsv_taetigkeit", 'Anmerkung'=>"+scsv_anmerkung", 'Ges.-Std.'=>"scsv_gesstd",
					'Ver.-Std.'=>"scsv_verstd", '+Einzelkosten'=>"scsv_einzelkosten", '+Gesamtkosten'=>"scsv_gesamtkosten", '+Einzelpreis'=>"scsv_einzelpreis",
					'+Gesamtpreis'=>"scsv_gesamtpreis", '+Rohertrag'=>"scsv_rohertrag", 'Positionsart'=>"+scsv_positionsart", 'Reisenummer'=>"+scsv_reisenummer",
					'Einzelbelegart'=>"+scsv_einzelbelegart", 'Umsatzsteuersatz'=>"scsv_umsatzsteuersatz", 'Umsatzsteuerland'=>"+scsv_umsatzsteuerland"
				);

		$table ='<table border="0" cellpadding="2" cellspacing="1" bgcolor="#000000">'.$GLOBALS['nl'];
		$table .= '<tr>';
		$ccnt=0;
		foreach($carr as $chead => $cvar)
		{
			$rpos=false;
			if(substr((string)$chead, 0, 1) == '+')
			{
				$chead=substr((string)$chead, 1);
				$rpos=true;
			}
			if(!$rpos || $rechnid)
			{
				if($ccnt)
					$scsv .= ';';
				$scsv .= '"'.$chead.'"';

				$table .= '<td nowrap bgcolor="#ccccff"><b>'.$chead.'</b></td>';

				$ccnt++;
			}
		}
		$scsv .= $GLOBALS['nl'];
		$table .= '</tr>'.$GLOBALS['nl'];

		foreach($Person as $pers)
		{
			$pers_id=(int)$pers['personen_id'];

			// ...Name
			$scsv_name=trim((string)$pers['pname']);
			$tmp_vorname=trim((string)$pers['vorname']);
			if(strlen((string)$tmp_vorname))
				$scsv_name .= ', '.$tmp_vorname;

			// ...Mitarbeiter-Identifikations-Nr.
			$scsv_ident=trim((string)$pers['personen_initialen']);

			foreach($zdata as $z)
			{
				$zart=(int)$z['zeitart'];
				$land=(int)$z['land_id'];

				// Nur Daten zu Aufgaben der aktuellen Person
				if(((int)$z['personen_id'] == $pers_id) && ($z['projekt_id'] > 0))
				{
					$repeat=1;
					while($repeat > 0)
					{
						if($zart == FXP_TE_TRAVELEXP_TIMEREC)
							$zart=FXP_TE_TIMEREC;
						else
							$repeat=0;

						if((!$rechnid && (($zart == FXP_TE_TIMEREC) || ($zart == FXP_TE_TIMEREC_SUM) || (($zart == FXP_TE_TRAVELEXP) && $_POST['Reisedaten']))) || ($rechnid && (((($zart == FXP_TE_TIMEREC) || ($zart == FXP_TE_TIMEREC_SUM)) && ($z['rechnungs_id'] == $rechnid)) || (($zart == FXP_TE_TRAVELEXP) && ($z['rechnungs_id_rk'] == $rechnid)))))
						{
							// Kalkulationen
							$hset_ek=0.0;
							$hset_vk=0.0;
							if(($zart == FXP_TE_TIMEREC) || ($zart == FXP_TE_TIMEREC_SUM))
							{
								if($z['zeitaufwand'])
								{
									$hset_ek=round_hset($z['personen_id'], $z['projekt_id'], round($z['kosten_ek']*3600.0/$z['zeitaufwand'], 2), false, false);
									$hset_vk=round_hset($z['personen_id'], $z['projekt_id'], round($z['kosten_vk']*3600.0/$z['zeitaufwand'], 2), false, true);
								}
								$kst_ek=round($z['zeitaufwand']*$hset_ek/3600.0, $cnks);
								$kst_vk=round($z['zeitaufwand']*$hset_vk/3600.0, $cnks);
							}
							else
							{
								$kst_ek=round($z['rk_ek'], $cnks);
								$kst_vk=round($z['rk_vk'], $cnks);
							}
//echo($scsv_name.': $hset_ek='.$hset_ek.' -&gt; '.$kst_ek.', $hset_vk='.$hset_vk.' -&gt; '.$kst_vk.'<br />');

							// ...Datum
							$sdatum=get_entry($z['ist_beginn'], 'datum');
							$edatum=get_entry($z['ist_ende'], 'datum');
							$scsv_datum=$sdatum;
							if(($z['ist_ende'] !== NULL) && strlen((string)$z['ist_ende']) && ($edatum != $sdatum))
								$scsv_datum .= '-'.$edatum;

							// ...Wochentag
							$sday=((int)$weekdays[substr((string)$z['ist_beginn'], 0,8)]+6) % 7;
							$eday=$sday;
							if($z['ist_ende'] !== NULL)
								$eday=((int)$weekdays[substr((string)$z['ist_ende'], 0,8)]+6) % 7;
							$scsv_wochentag=$GLOBALS['wochentagsnamen'][$sday];
							if(($z['ist_ende'] !== NULL) && strlen((string)$z['ist_ende']) && ($eday != $sday))
								$scsv_wochentag .= '-'.$GLOBALS['wochentagsnamen'][$eday];

							// ...Zeit von + Zeit bis
							$scsv_zeitvon=get_entry($z['ist_beginn'], 'zeit');
							$scsv_zeitbis=get_entry($z['ist_ende'], 'zeit');
							if($scsv_zeitvon == $scsv_zeitbis)
							{
								$scsv_zeitvon='';
								$scsv_zeitbis='';
							}

							// ...Pause
							$scsv_pause=get_entry($z['pause']/3600, 'dezimal');

							// ...1. Vorgangsebene (= Mutterprojekt)
							$vn=substr((string)$z['vorgangsnummer'], 0, 4);
							$scsv_vorgang1='';
							if((strlen((string)$vn) == 4) && isset($Projekte[$vn]))
								$scsv_vorgang1=no_null($vn).' - '.$Projekte[$vn]['name_projekt'];

							// ...2. Vorgangsebene
							$vn=substr((string)$z['vorgangsnummer'], 0, 9);
							$scsv_vorgang2='';
							if((strlen((string)$vn) == 9) && isset($Projekte[$vn]))
								$scsv_vorgang2=no_null($vn).' - '.$Projekte[$vn]['name_projekt'];

							// ...3. Vorgangsebene
							$vn=substr((string)$z['vorgangsnummer'], 0, 14);
							$scsv_vorgang3='';
							if((strlen((string)$vn) == 14) && isset($Projekte[$vn]))
								$scsv_vorgang3=no_null($vn).' - '.$Projekte[$vn]['name_projekt'];

							// ...4. Vorgangsebene
							$vn=substr((string)$z['vorgangsnummer'], 0, 19);
							$scsv_vorgang4='';
							if((strlen((string)$vn) == 19) && isset($Projekte[$vn]))
								$scsv_vorgang4=no_null($vn).' - '.$Projekte[$vn]['name_projekt'];

							// ...5. Vorgangsebene
							$vn=substr((string)$z['vorgangsnummer'], 0, 24);
							$scsv_vorgang5='';
							if((strlen((string)$vn) == 24) && isset($Projekte[$vn]))
								$scsv_vorgang5=no_null($vn).' - '.$Projekte[$vn]['name_projekt'];

							// ...Aufgabe
							$vn=$z['vorgangsnummer'];
							$scsv_aufgabe=no_null($vn).' - '.$Projekte[$vn]['name_projekt'];

							// ...Referenznummer
							if($Projekte[$vn]['ref_nummer'] !== NULL)
								$scsv_referenznummer=str_replace('"', "'", trim((string)$Projekte[$vn]['ref_nummer']));
							else
								$scsv_referenznummer='';

							// ...Tätigkeit
							if($z['taetigkeit_text'] !== NULL)
								$scsv_taetigkeit=str_replace('"', "'", trim((string)$z['taetigkeit_text']));
							else
								$scsv_taetigkeit='';

							// ...Anmerkung
							if($z['anmerkungintern'] !== NULL)
								$scsv_anmerkung=str_replace('"', "'", trim((string)$z['anmerkungintern']));
							else
								$scsv_anmerkung='';

							// ...Ges.-Std.
							$scsv_gesstd=$dez0;
							if(($zart == FXP_TE_TIMEREC) || ($zart == FXP_TE_TIMEREC_SUM))
								$scsv_gesstd=get_entry($z['zeitaufwand']/3600.0, 'dezimal');

							// ...Ver.-Std.
							$scsv_verstd=$scsv_gesstd;

							// ...Einzelkosten
							$scsv_einzelkosten=get_entry($hset_ek, 'dezimal');

							// ...Gesamtkosten
							$scsv_gesamtkosten=get_entry($kst_ek, 'dezimal', $cnks);

							// ...Einzelpreis
							$scsv_einzelpreis=get_entry($hset_vk, 'dezimal');

							// ...Gesamtpreis
							$scsv_gesamtpreis=get_entry($kst_vk, 'dezimal', $cnks);

							// ...Rohertrag=Gesamtpreis - Gesamtkosten
							$scsv_rohertrag=get_entry($kst_vk-$kst_ek, 'dezimal', $cnks);

							// Positionsart: 01=Projektzeiten, 02=Reisekosten
							$scsv_positionsart='01';
							if($zart == FXP_TE_TRAVELEXP)
								$scsv_positionsart='02';

							// Reisenummer: Nummer der zugeordneten Reise falls Reisekosten und Nummer vorhanden, sonst -leer-
							if(($zart == FXP_TE_TIMEREC) || ($zart == FXP_TE_TIMEREC_SUM))
								$scsv_reisenummer='';
							else
							{
								if($z['zeiteinheit'])
									$scsv_reisenummer=db_value("SELECT belegnr FROM zeitdaten WHERE zeit_id=".$z['zeiteinheit']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
								else if($z['belegnr'] !== NULL)
									$scsv_reisenummer=trim((string)$z['belegnr']);
								if(($scsv_reisenummer === NULL) || !strlen((string)$scsv_reisenummer))
									$scsv_reisenummer='';
							}

							// Einzelbelegart: Art des Einzelbelegs falls Belegsposition und Reisekosten, sonst -leer-
							$scsv_einzelbelegart='';
							if(($zart == FXP_TE_TRAVELEXP) && $z['zeiteinheit'])
								$scsv_einzelbelegart=$rk_kat[$z['rk_kategorie']];

							// Umsatzsteuersatz: Prozentualwert der Umsatzsteuer
							$id='';
							if(isset($z['rechnungs_id']) && $z['rechnungs_id'] && isset($z['positions_id']) && $z['positions_id'])
								$id=(int)$z['rechnungs_id'].'_'.(int)$z['positions_id'];
							else if(isset($z['rechnungs_id_rk']) && $z['rechnungs_id_rk'] && isset($z['positions_id_rk']) && $z['positions_id_rk'])
								$id=(int)$z['rechnungs_id_rk'].'_'.(int)$z['positions_id_rk'];
							if(strlen((string)$id) && isset($rechnps[$id]))
								$scsv_umsatzsteuersatz=$rechnps[$id]['mwst_satz'];
							else
							{
								$scsv_umsatzsteuersatz=$dez0;
								if(($zart == FXP_TE_TIMEREC) || ($zart == FXP_TE_TIMEREC_SUM))
									$scsv_umsatzsteuersatz=$mwst;
								else if($z['zeiteinheit'] && ($z['rk_kategorie'] != 1980))
									$scsv_umsatzsteuersatz=get_entry($z['mwst_satz'], 'dezimal');
							}

							// Umsatzsteuerland: Bei Reisekosten gleich dem Reise- bzw. Belegsland falls definiert, sonst -leer-
							$scsv_umsatzsteuerland='';
							if(($zart == FXP_TE_TRAVELEXP) && $land)
							{
								if(!isset($rk_land[$land]))
									$rk_land[$land]=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$land." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
								$scsv_umsatzsteuerland=$rk_land[$land];
							}

							if(((($zart == FXP_TE_TIMEREC) || ($zart == FXP_TE_TIMEREC_SUM)) && $z['nicht_abrechenbar']) || (($zart == FXP_TE_TRAVELEXP) && $z['rk_nicht_abrechenbar']))
							{
								$scsv_verstd=$dez0;
								$scsv_einzelpreis=$dez0;
								$scsv_gesamtpreis=$dez1;
								$scsv_rohertrag=get_entry(0.0-$kst_ek, 'dezimal', $cnks);
							}

							// Datenzeile
							$table .= '<tr>';
							$ccnt=0;
							foreach($carr as $chead => $cvar)
							{
								$rpos=false;
								if(substr((string)$chead, 0, 1) == '+')
								{
									$chead=substr((string)$chead, 1);
									$rpos=true;
								}
								$qm='';
								if(substr((string)$cvar, 0, 1) == '+')
								{
									$qm='"';
									$cvar=substr((string)$cvar, 1);
								}
								if(!$rpos || $rechnid)
								{
									if($ccnt)
										$scsv .= ';';
									$scsv .= $qm.${$cvar}.$qm;

									$table .= '<td nowrap bgcolor="#ffffff">'.$qm.${$cvar}.$qm.'</td>';

									$ccnt++;
								}
							}
							$scsv .= $GLOBALS['nl'];
							$table .= '</tr>'.$GLOBALS['nl'];

							// Reisekosten-Zusatzdaten für Leistungsnachweis (Tagesangaben + Belege)
							if(!$rechnid && ($zart == FXP_TE_TRAVELEXP) && ($z['rk_art'] != 1983))
							{
								// ...Tagesangaben
								$sql ="SELECT z.zeit_id, z.rk_art, z.land_id, z.rk_ek, z.rk_vk, z.rk, z.rk_nicht_abrechenbar,";
								$sql .= " t.taetigkeit_text, t.anmerkungintern";
								$sql .= " FROM zeitdaten z, taetigkeiten t";
								$sql .= " WHERE z.rk_kategorie=1980 AND z.zeiteinheit=".$z['zeit_id']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND t.zeit_id=z.zeit_id AND t.mandanten_id=".$GLOBALS['fxpglobals']['client'];
								$angaben=db_values($sql);
								if(is_array($angaben))
								{
//fxDebug($angaben, $sql);
									$land	= (int)$angaben['land_id'];
									$kst_ek=round($angaben['rk_ek'], $cnks);
									$kst_vk=round($angaben['rk_vk'], $cnks);

									// ...Pause
									$scsv_pause=$dez0;

									// ...Tätigkeit
									$scsv_taetigkeit='';
									if(!is_null($angaben['taetigkeit_text']) && strlen((string)$angaben['taetigkeit_text']))
										$scsv_taetigkeit=str_replace('"', "'", trim((string)$angaben['taetigkeit_text']));

									// ...Anmerkung
									$scsv_anmerkung='';
									if(!is_null($angaben['anmerkungintern']) && strlen((string)$angaben['anmerkungintern']))
										$scsv_anmerkung=str_replace('"', "'", trim((string)$angaben['anmerkungintern']));

									// ...Ges.-Std.
									$scsv_gesstd=$dez0;

									// ...Ver.-Std.
									$scsv_verstd=$scsv_gesstd;

									// ...Einzelkosten
									$scsv_einzelkosten=$dez0;

									// ...Gesamtkosten
									$scsv_gesamtkosten=get_entry($kst_ek, 'dezimal', $cnks);

									// ...Einzelpreis
									$scsv_einzelpreis=$dez0;

									// ...Gesamtpreis
									$scsv_gesamtpreis=get_entry($kst_vk, 'dezimal', $cnks);

									// ...Rohertrag=Gesamtpreis - Gesamtkosten
									$scsv_rohertrag=get_entry($kst_vk-$kst_ek, 'dezimal', $cnks);

									// Einzelbelegart: Art des Einzelbelegs falls Belegsposition und Reisekosten, sonst -leer-
									$scsv_einzelbelegart=$rk_kat[1980];

									// Umsatzsteuersatz: Prozentualwert der Umsatzsteuer
									$id='';
									if(isset($z['rechnungs_id_rk']) && $z['rechnungs_id_rk'] && isset($z['positions_id_rk']) && $z['positions_id_rk'])
										$id=(int)$z['rechnungs_id_rk'].'_'.(int)$z['positions_id_rk'];
									if(strlen((string)$id) && isset($rechnps[$id]))
										$scsv_umsatzsteuersatz=$rechnps[$id]['mwst_satz'];
									else
										$scsv_umsatzsteuersatz=$dez0;

									// Umsatzsteuerland: Bei Reisekosten gleich dem Reise- bzw. Belegsland falls definiert, sonst -leer-
									$scsv_umsatzsteuerland='';
									if($land)
									{
										if(!isset($rk_land[$land]))
											$rk_land[$land]=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$land." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
										$scsv_umsatzsteuerland=$rk_land[$land];
									}

									if($angaben['rk_nicht_abrechenbar'])
									{
										$scsv_einzelpreis=$dez0;
										$scsv_gesamtpreis=$dez1;
										$scsv_rohertrag=get_entry(0.0-$kst_ek, 'dezimal', $cnks);
									}

									// Datenzeile
									$table .= '<tr>';
									$ccnt=0;
									foreach($carr as $chead => $cvar)
									{
										$rpos=false;
										if(substr((string)$chead, 0, 1) == '+')
										{
											$chead=substr((string)$chead, 1);
											$rpos=true;
										}
										$qm='';
										if(substr((string)$cvar, 0, 1) == '+')
										{
											$qm='"';
											$cvar=substr((string)$cvar, 1);
										}
										if(!$rpos || $rechnid)
										{
											if($ccnt)
												$scsv .= ';';
											$scsv .= $qm.${$cvar}.$qm;

											$table .= '<td nowrap bgcolor="#ffffff">'.$qm.${$cvar}.$qm.'</td>';

											$ccnt++;
										}
									}
									$scsv .= $GLOBALS['nl'];
									$table .= '</tr>'.$GLOBALS['nl'];
								}

								// ...Einzelbelege
								$sql ="SELECT z.zeit_id, z.ist_beginn, z.ist_ende, z.rk_art, z.rk_kategorie, z.land_id, z.rk_ek, z.rk_vk, z.rk, z.mwst_satz, z.belegnr, z.rk_nicht_abrechenbar,";
								$sql .= " t.taetigkeit_text, t.anmerkungintern";
								$sql .= " FROM zeitdaten z, taetigkeiten t";
								$sql .= " WHERE z.rk_kategorie<>1980 AND z.zeiteinheit=".$z['zeit_id']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND t.zeit_id=z.zeit_id AND t.mandanten_id=".$GLOBALS['fxpglobals']['client'];
								$sql .= " ORDER BY z.belegnr, z.zeit_id";
								$belege=db_values($sql, '*');
								if(is_array($belege))
								{
//fxDebug($belege, $sql);
									foreach($belege as $bel)
									{
										$land	= (int)$bel['land_id'];
										$kst_ek=round($bel['rk_ek'], $cnks);
										$kst_vk=round($bel['rk_vk'], $cnks);

										// ...Datum
										$sdatum=get_entry($bel['ist_beginn'], 'datum');
										$edatum=get_entry($bel['ist_ende'], 'datum');
										$scsv_datum=$sdatum;
										if(strlen((string)$edatum) && ($edatum != $sdatum))
											$scsv_datum .= '-'.$edatum;

										// ...Wochentag
										$day=((int)$weekdays[substr((string)$bel['ist_beginn'], 0, 8)]+6) % 7;
										$scsv_wochentag=$GLOBALS['wochentagsnamen'][$day];

										// ...Zeit von + Zeit bis
										$scsv_zeitvon='';
										$scsv_zeitbis='';

										// ...Pause
										$scsv_pause=$dez0;

										// ...Tätigkeit
										$scsv_taetigkeit='';
										if(!is_null($bel['taetigkeit_text']) && strlen((string)$bel['taetigkeit_text']))
											$scsv_taetigkeit=str_replace('"', "'", trim((string)$bel['taetigkeit_text']));

										// ...Anmerkung
										$scsv_anmerkung='';
										if(!is_null($bel['anmerkungintern']) && strlen((string)$bel['anmerkungintern']))
											$scsv_anmerkung=str_replace('"', "'", trim((string)$bel['anmerkungintern']));

										// ...Ges.-Std.
										$scsv_gesstd=$dez0;

										// ...Ver.-Std.
										$scsv_verstd=$scsv_gesstd;

										// ...Einzelkosten
										$scsv_einzelkosten=$dez0;

										// ...Gesamtkosten
										$scsv_gesamtkosten=get_entry($kst_ek, 'dezimal', $cnks);

										// ...Einzelpreis
										$scsv_einzelpreis=$dez0;

										// ...Gesamtpreis
										$scsv_gesamtpreis=get_entry($kst_vk, 'dezimal', $cnks);

										// ...Rohertrag=Gesamtpreis - Gesamtkosten
										$scsv_rohertrag=get_entry($kst_vk-$kst_ek, 'dezimal', $cnks);

										// Einzelbelegart: Art des Einzelbelegs falls Belegsposition und Reisekosten, sonst -leer-
										$scsv_einzelbelegart=$rk_kat[$bel['rk_kategorie']];

										// Umsatzsteuersatz: Prozentualwert der Umsatzsteuer
										$id='';
										if(isset($z['rechnungs_id_rk']) && $z['rechnungs_id_rk'] && isset($z['positions_id_rk']) && $z['positions_id_rk'])
											$id=(int)$z['rechnungs_id_rk'].'_'.(int)$z['positions_id_rk'];
										if(strlen((string)$id) && isset($rechnps[$id]))
											$scsv_umsatzsteuersatz=$rechnps[$id]['mwst_satz'];
										else
											$scsv_umsatzsteuersatz=get_entry($bel['mwst_satz'], 'dezimal');

										// Umsatzsteuerland: Bei Reisekosten gleich dem Reise- bzw. Belegsland falls definiert, sonst -leer-
										$scsv_umsatzsteuerland='';
										if($land)
										{
											if(!isset($rk_land[$land]))
												$rk_land[$land]=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$land." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
											$scsv_umsatzsteuerland=$rk_land[$land];
										}

										if($bel['rk_nicht_abrechenbar'])
										{
											$scsv_einzelpreis=$dez0;
											$scsv_gesamtpreis=$dez1;
											$scsv_rohertrag=get_entry(0.0-$kst_ek, 'dezimal', $cnks);
										}

										// Datenzeile
										$table .= '<tr>';
										$ccnt=0;
										foreach($carr as $chead => $cvar)
										{
											$rpos=false;
											if(substr((string)$chead, 0, 1) == '+')
											{
												$chead=substr((string)$chead, 1);
												$rpos=true;
											}
											$qm='';
											if(substr((string)$cvar, 0, 1) == '+')
											{
												$qm='"';
												$cvar=substr((string)$cvar, 1);
											}
											if(!$rpos || $rechnid)
											{
												if($ccnt)
													$scsv .= ';';
												$scsv .= $qm.${$cvar}.$qm;

												$table .= '<td nowrap bgcolor="#ffffff">'.$qm.${$cvar}.$qm.'</td>';

												$ccnt++;
											}
										}
										$scsv .= $GLOBALS['nl'];
										$table .= '</tr>'.$GLOBALS['nl'];
									}
								}
							}
						}

						if($repeat > 0)
							$zart=FXP_TE_TRAVELEXP;
					}
				}
			}
		}
		$table .= '</table>'.$GLOBALS['nl'];
//echo('<pre>'.fxHtmlEncode($scsv).'</pre>'.$nl);
//echo($table);
	}

	// Spezialreport als Datei im TMP-Ordner abspeichern
	$GLOBALS['fxptdata']['stime']=substr('000'.$GLOBALS['fxpglobals']['tr'], -3).fx_date('YmdHis');
	$sprep_filename=$GLOBALS['tmppath'].'~'.substr((string)$GLOBALS['fxptdata']['stime'],3).$GLOBALS['fxpglobals']['person'].'.csv';
	if(strlen((string)$scsv))
		fxSave($sprep_filename, $scsv);
	else
		fxDelete($sprep_filename);
}

/**
 * ???FUNCTION???
 *
 * @param various $id     - Mandatory parameter:                   ???PARAMETER???
 * @param various $prj_id - Mandatory parameter:                   ???PARAMETER???
 * @param various $hs     - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $mat    - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $vk     - Optional parameter (default = true):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function round_hset($id, $prj_id, $hs, $mat=false, $vk=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($vk)
		$cpp='vk';
	else
		$cpp='ek';
	if($mat)
		$cpp .= 'm';
	else
		$cpp .= 'p';
	$cpp .= '_'.$id.'_'.$prj_id;
	if(!isset($GLOBALS['hset'][$cpp]))
	{
		if($prj_id)
		{
			$sql ="SELECT a.ek_projekt AS ek, a.vk_projekt AS vk";
			$sql .= " FROM artikelzuordnung a, maplanung m";
			$sql .= " WHERE m.projekt_id=".$prj_id." AND m.";
			if($mat)
				$sql .= "sachmittel_id";
			else
				$sql .= "personen_id";
			$sql .= "=".$id." AND a.maplanung_id=m.maplanung_id AND a.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		}
		else
		{
			$sql ="SELECT ek, vk";
			$sql .= " FROM artikel";
			$sql .= " WHERE ";
			if($mat)
				$sql .= "sachmittel_id";
			else
				$sql .= "personen_id";
			$sql .= "=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (archiv=0 OR archiv IS NULL)";
		}
		$cekvk=db_values($sql);
		if(is_array($cekvk))
		{
			if($vk)
				$GLOBALS['hset'][$cpp]=$cekvk['vk'];
			else
				$GLOBALS['hset'][$cpp]=$cekvk['ek'];
		}
		else
			$GLOBALS['hset'][$cpp]=$hs;
//fxDebug($GLOBALS['hset'], $sql);
	}

//echo('<font class=blue>$id='.$id.', $prj_id='.$prj_id.', $hs='.$hs.'</font><br />'.$GLOBALS['nl']);
	if(($hs > $GLOBALS['hset'][$cpp]-0.05) && ($hs < $GLOBALS['hset'][$cpp]+0.05))
		$hs=$GLOBALS['hset'][$cpp];
//echo('<font class=blue>-&gt; $hs='.$hs.'</font><br />'.$GLOBALS['nl']);

	return $hs;
}

/**
 * ???FUNCTION???
 *
 * @param various $url  - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $post - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getPageCURL($url,$post=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ra=array('error'=>"cURL extension not loaded", 'content'=>"");
	if(extension_loaded('curl'))
	{
		$ch=curl_init();
		if($ch !== false)
		{
			// Set URL and other options
			curl_setopt($ch, CURLOPT_URL, $url);
			curl_setopt($ch, CURLOPT_HEADER, false);
			curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
			curl_setopt($ch, CURLOPT_AUTOREFERER, true);
			curl_setopt($ch, CURLOPT_COOKIESESSION, true);
			curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
			curl_setopt($ch, CURLOPT_FRESH_CONNECT, true);
			curl_setopt($ch, CURLOPT_USERAGENT, $_SERVER['HTTP_USER_AGENT']);
			curl_setopt($ch, CURLOPT_TIMEOUT, 20);
			curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
			curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);

			// Use Proxy?
			if(fxIsArray($GLOBALS['fxpglobals']['dbparam']) && isset($GLOBALS['fxpglobals']['dbparam']['curl_proxy']) && strlen((string)$GLOBALS['fxpglobals']['dbparam']['curl_proxy']))
			{
				if(!fxIsArray($GLOBALS['fxdb']) || !isset($GLOBALS['fxdb']['username']) || !strlen((string)$GLOBALS['fxdb']['username']))
					fxf_dbGetCP();
				curl_setopt($ch, CURLOPT_PROXY, $GLOBALS['fxpglobals']['dbparam']['curl_proxy']);
				if(isset($GLOBALS['fxpglobals']['dbparam']['curl_proxy_port']) && strlen((string)$GLOBALS['fxpglobals']['dbparam']['curl_proxy_port']))
				{
					$port=(int)$GLOBALS['fxpglobals']['dbparam']['curl_proxy_port'];
					if($port > 0)
						curl_setopt($ch, CURLOPT_PROXYPORT, $port);
				}

				if(fxIsArray($GLOBALS['fxdb']) && isset($GLOBALS['fxdb']['curl_proxy_username']) && strlen((string)$GLOBALS['fxdb']['curl_proxy_username']))
				{
					$proxyuserpwd=$GLOBALS['fxdb']['curl_proxy_username'];
					if(isset($GLOBALS['fxdb']['curl_proxy_password']) && strlen((string)$GLOBALS['fxdb']['curl_proxy_password']))
						$proxyuserpwd .= ':'.$GLOBALS['fxdb']['curl_proxy_password'];
					curl_setopt($ch, CURLOPT_PROXYUSERPWD, $proxyuserpwd);
				}
			}

			// Post data
			if(fxIsArray($post))
			{
				curl_setopt($ch, CURLOPT_POST, 1);
				curl_setopt($ch, CURLOPT_POSTFIELDS, $post);
			}

			// Grab URL and pass it to the browser
			$ra['content']=curl_exec($ch);
			if($ra['content'] !== false)
			{
				if(!strlen((string)$ra['content']))
				{
					$ra['content']="";
					$ra['error']='No data returned';
				}
				else
				{
					$ttsp=stripos($ra['content'],'<title>');
					if($ttsp !== false)
					{
						$ttep=stripos($ra['content'],'</title>',$ttsp);
						$ra['error']=trim(substr((string)$ra['content'],$ttsp+7,$ttep-$ttsp-7));
						$ra['content']="";
					}
					else
						$ra['error']=false;
				}
			}
			else
			{
				$ra['error']=curl_error($ch);
				$ra['content']="";
			}

			// Close cURL resource, and free up system resources
			curl_close($ch);
		}
		else
			$ra['error']="cURL could not be initialized";
	}
//fxDebug($ra,$url);

	return $ra;
}

/**
 * Funktion zum Herstellen der Auswahlwerte für die Rechnung
 *
 * @param various $afa     - Mandatory parameter:               ???PARAMETER???
 * @param various $usepost - Mandatory parameter:               ???PARAMETER???
 * @param integer $smnr    - Optional parameter (default = 0):  ???PARAMETER???
 */
function setInvoiceSelectVars($afa, $usepost, $smnr=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(fxIsArray($afa))
	{
		foreach($afa as $afm => $afd)
		{
			foreach($afd as $fld)
			{
				$fty=substr((string)$fld, 0, 1);
				$fld=substr((string)$fld, 1);

				$backup2post=true;
				if($usepost)
				{
					// Allgemeine oder Spezielle
					if(!$afm || ($afm == $usepost))
					{
						$backup2post=false;
						if(isset($_POST[$fld]))
							$GLOBALS['fxptdata']['backup'][$fld]=$_POST[$fld];
						else if($fty == 'c')
							$GLOBALS['fxptdata']['backup'][$fld]=0;
					}
				}
				if($backup2post && isset($GLOBALS['fxptdata']['backup'][$fld]) && ((($fty == 'c') && $GLOBALS['fxptdata']['backup'][$fld]) || ($fty != 'c')))
					$_POST[$fld]=$GLOBALS['fxptdata']['backup'][$fld];

				if(isset($_POST[$fld]))
					$GLOBALS[$fld]=$_POST[$fld];
				else if(isset($GLOBALS[$fld]))
					unset($GLOBALS[$fld]);

				if($smnr)
					$_POST['sf'.$smnr.'_'.$fld]=$fty;
			}
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various &$arr - Mandatory parameter:                   ???PARAMETER???
 * @param various $k    - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $desc - Optional parameter (default = false):  ???PARAMETER???
 */
function sortSubArray(&$arr, $k, $desc=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	foreach($arr as &$v)
		$v['(dummykey)']=$v[$k];
	if(!$desc)
		usort($arr, 'sortSubArrayFunctionAsc');
	else
		usort($arr, 'sortSubArrayFunctionDesc');
	foreach($arr as &$v)
		unset($v['(dummykey)']);
}

/**
 * ???FUNCTION???
 *
 * @param various $a - Mandatory parameter: ???PARAMETER???
 * @param various $b - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sortSubArrayFunctionAsc($a, $b)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($a['(dummykey)'] == $b['(dummykey)'])
		return 0;
    if(strtolower((string)$a['(dummykey)']) < strtolower((string)$b['(dummykey)']))
    	return -1;
    return 1;
}

/**
 * ???FUNCTION???
 *
 * @param various $a - Mandatory parameter: ???PARAMETER???
 * @param various $b - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sortSubArrayFunctionDesc($a, $b)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($a['(dummykey)'] == $b['(dummykey)'])
		return 0;
    if(strtolower((string)$a['(dummykey)']) < strtolower((string)$b['(dummykey)']))
    	return 1;
    return -1;
}

/**
 * ???FUNCTION???
 *
 * @param various $val   - Mandatory parameter:                ???PARAMETER???
 * @param string  $mode  - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $dummy - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $unit  - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function gVal($val, $mode='', $dummy='', $unit='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen((string)$mode))
		$mode='zeitspanne';
	if(strlen((string)$unit))
		$unit='&nbsp;'.$unit;
	$mco='';
	$mcc='';
	if($dummy == 'lg')
	{
		$mco='<font class="lightergrey">';
		$mcc='</font>';
	}
	else if($dummy == 'l')
	{
		$mco='<font class="lightgrey">';
		$mcc='</font>';
	}
	else if($dummy == 'g')
	{
		$mco='<font class="grey">';
		$mcc='</font>';
	}
	else if(strlen((string)$dummy))
	{
		$mco='<i class="lightgrey">(';
		if($val < 0.0)
			$mco='<i class="lightred">(';
		$mcc=')</i>';
	}
	else if($val < 0.0)
	{
		$mco='<font class="red">';
		$mcc='</font>';
	}

	return $mco.get_entry($val, $mode).$unit.$mcc;
}

/**
 * ???FUNCTION???
 *
 * @param various $tid             - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $with_allowances - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $with_vouchers   - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getTravelData($tid, $with_allowances=true, $with_vouchers=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$tda=array(
		'tid'=>0, 'cust'=>0, 'type'=>1984, 'cat'=>1966, 'lid'=>0, 'land'=>"", 'sdate'=>"00000000000000", 'edate'=>"99999999999999", 'splace'=>"", 'dplace'=>"", 'km'=>0, 'net'=>0.0, 'vrate'=>0.0, 'gross'=>0.0, 'nninv'=>"",
		'aid'=>0, 'atext'=>"", 'anote'=>"",
		'pid'=>0, 'pno'=>"", 'pname'=>meldung(681),	// Unbekannt

		'days'=>0, 'hours'=>0, 'maxh'=>0, 'maxd'=>'', 'atot'=>0.0,

		'allowances'=>array(
			'8h'=>0.0, '14h'=>0.0, '24h'=>0.0, 'night'=>0.0
		),

		'boards'=>array(),

		'accommodations'=>array(),

		'breakfasts'=>array(),
		'lunches'=>array(),
		'suppers'=>array(),

		'meals'=>array(
			'breakfast'=>array('amount'=>0.0, 'value'=>0.0, 'unit'=>$GLOBALS['fxpglobals']['settings']['waehrung']),
			'lunch'=>array('amount'=>0.0, 'value'=>0.0, 'unit'=>$GLOBALS['fxpglobals']['settings']['waehrung']),
			'supper'=>array('amount'=>0.0, 'value'=>0.0, 'unit'=>$GLOBALS['fxpglobals']['settings']['waehrung'])
		),

		'vouchers'=>array(),

		'dates'=>array()
	);

	// Get travel data
	$sql  = "SELECT z.projekt_id, z.zkunde, z.ist_beginn, z.ist_ende, z.rk_art, z.rk_kategorie, z.rk_ort1, z.rk_ort2, z.rk_km, z.rk_ek, z.mwst_satz, z.rk, z.land_id, z.rk_nicht_abrechenbar,";
	$sql .= " t.taetigkeit_id, t.taetigkeit_text, t.anmerkungintern ";
	$sql .= "FROM zeitdaten z, taetigkeiten t ";
	$sql .= "WHERE z.zeit_id=".$tid." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND t.zeit_id=z.zeit_id AND t.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$rda=db_values($sql);
//fxDebug($rda,$sql, 0);
	if(fxIsArray($rda))
	{

		// ...project
		$tda['pid']=(int)$rda['projekt_id'];
		if($tda['pid'])
		{
			$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$tda['pid']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pda=db_values($sql);
//fxDebug($pda,$sql, 0);
			if(fxIsArray($pda))
			{
				$tda['pno']=$pda['vorgangsnummer'];
				$tda['pname']=$pda['name_projekt'];
			}
		}

		// ...travel
		$tda['tid']=$tid;
		$tda['cust']=(int)$rda['zkunde'];
		$tda['type']=(int)$rda['rk_art'];
		$tda['cat']=(int)$rda['rk_kategorie'];
		$tda['lid']=(int)$rda['land_id'];
		$tda['sdate']=$rda['ist_beginn'];
		$tda['edate']=$rda['ist_ende'];
		$tda['splace']=$rda['rk_ort1'];
		$tda['dplace']=$rda['rk_ort2'];
		$tda['km']=(int)$rda['rk_km'];
		$tda['net']=(float)$rda['rk_ek'];
		$tda['vrate']=(float)$rda['mwst_satz'];
		$tda['gross']=(float)$rda['rk'];
		if($rda['rk_nicht_abrechenbar'])
			$tda['ninv']="1";

		// Adjust old behaviour
		if(!$tda['type'])
		{
			$tda['type']=1984;	// ...Various
			$tda['cat']=1966;	// ...No vehicle
		}

		// ...action
		$tda['aid']=(int)$rda['taetigkeit_id'];
		$tda['atext']=$rda['taetigkeit_text'];
		$tda['anote']=$rda['anmerkungintern'];
	}

	// Land
	if($tda['lid'])
		$tda['land']=db_value("SELECT land_ort FROM rkpauschalen WHERE land_id=".$tda['lid']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

	// Allowances
	if($with_allowances)
	{
		$rsd=substr((string)$tda['sdate'],0,8).'000000';
		$red=substr((string)$tda['edate'],0,8).'240000';

		// Get daily board/accomodation allowance values
		if($tda['lid'])
		{
			$sql  = "SELECT p8h, p14h, p24h, pnacht FROM rkpauschalen WHERE land_id=".$tda['lid'];
			$sql .= " AND (gueltig_von IS NULL OR gueltig_von<='".$red."') AND (gueltig_bis IS NULL OR gueltig_bis>='".$rsd."')";
			$sql .= " AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$tmp=db_values($sql);
//fxDebug($tmp,$sql, 0);
			if(fxIsArray($tmp))
			{
				$tda['allowances']['8h']=get_double($tmp['p8h']);
				$tda['allowances']['14h']=get_double($tmp['p14h']);
				$tda['allowances']['24h']=get_double($tmp['p24h']);
				$tda['allowances']['night']=get_double($tmp['pnacht']);
			}
		}

		// Get existing daily board/accommodation/meal allowances
		$sql="SELECT zeit_id FROM zeitdaten WHERE zeiteinheit=".$tid." AND rk_kategorie=1980 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$did=(int)db_value($sql);
		if($did)
		{
			$sql="SELECT startzeit, pnacht, pverpflegung, pfrueh, pmittag, pabend FROM rktagesangaben WHERE zeit_id=".$did." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY startzeit";
			$daa=db_values($sql,'*');
//fxDebug($daa,$sql, 0);
			if(fxIsArray($daa))
			{
				foreach($daa as $da)
				{
					$dt=substr((string)$da['startzeit'],0,8);
					if($da['pverpflegung'])
						$tda['boards'][$dt]=$da['pverpflegung'];
					if($da['pnacht'])
						$tda['accommodations'][$dt]=$da['pnacht'];
					if($da['pfrueh'])
					{
						$tda['breakfasts'][$dt]=$da['pfrueh'];
						if(!$tda['meals']['breakfast']['amount'])
							$tda['meals']['breakfast']['amount']=$da['pfrueh'];
					}
					if($da['pmittag'])
					{
						$tda['lunches'][$dt]=$da['pmittag'];
						if(!$tda['meals']['lunch']['amount'])
							$tda['meals']['lunch']['amount']=$da['pmittag'];
					}
					if($da['pabend'])
					{
						$tda['suppers'][$dt]=$da['pabend'];
						if(!$tda['meals']['supper']['amount'])
							$tda['meals']['supper']['amount']=$da['pabend'];
					}
				}
			}
		}
//fxDebug($tda['meals'],'$tda[\'meals\']', 0);

		// ...Calculate remaining amounts if at least one amount is set
		if($tda['allowances']['24h'])
		{
			// ...Breakfast
			if(!$tda['meals']['breakfast']['amount'] && ($tda['meals']['lunch']['amount'] || $tda['meals']['supper']['amount']))
			{
				$tda['meals']['breakfast']['amount']=$tda['allowances']['24h'];
				if($tda['meals']['lunch']['amount'] && $tda['meals']['supper']['amount'])
					$tda['meals']['breakfast']['amount'] -= ($tda['meals']['lunch']['amount']+$tda['meals']['supper']['amount']);
				else if($tda['meals']['lunch']['amount'])
					$tda['meals']['breakfast']['amount'] -= ($tda['meals']['lunch']['amount'] * 2.0);
				else
					$tda['meals']['breakfast']['amount'] -= ($tda['meals']['supper']['amount'] * 2.0);
			}
			// ...Lunch
			if(!$tda['meals']['lunch']['amount'] && ($tda['meals']['breakfast']['amount'] || $tda['meals']['supper']['amount']))
			{
				$tda['meals']['lunch']['amount']=$tda['allowances']['24h'];
				if($tda['meals']['supper']['amount'])
					$tda['meals']['lunch']['amount']=$tda['meals']['supper']['amount'];
				else
					$tda['meals']['lunch']['amount']=($tda['meals']['lunch']['amount']-$tda['meals']['breakfast']['amount']) / 2.0;
			}
			// ...Supper
			if(!$tda['meals']['supper']['amount'] && ($tda['meals']['breakfast']['amount'] || $tda['meals']['lunch']['amount']))
			{
				$tda['meals']['supper']['amount']=$tda['allowances']['24h'];
				if($tda['meals']['lunch']['amount'])
					$tda['meals']['supper']['amount']=$tda['meals']['lunch']['amount'];
				else
					$tda['meals']['supper']['amount']=($tda['meals']['supper']['amount']-$tda['meals']['breakfast']['amount']) / 2.0;
			}
		}
		$tda['meals']['breakfast']['value']=$tda['meals']['breakfast']['amount'];
		$tda['meals']['lunch']['value']=$tda['meals']['lunch']['amount'];
		$tda['meals']['supper']['value']=$tda['meals']['supper']['amount'];
//fxDebug($tda['meals'],'$tda[\'meals\']', 0);

		// ...no subtraction of benefits anymore, instead use cutbacks (05.05.2015)
		if($tda['lid'] && (!$tda['meals']['breakfast']['amount'] || !$tda['meals']['lunch']['amount'] || !$tda['meals']['supper']['amount']))
		{
			if($tda['type'] == 1982)	// Abroad
			{
				// Breakfast
				if(!$tda['meals']['breakfast']['amount'])
				{
					// Percent
					$tda['meals']['breakfast']['value']=get_double($GLOBALS['fxpglobals']['settings']['rk_frueh_ausl']);
					$tda['meals']['breakfast']['unit']='%';
					// -> Amounts
					$tda['meals']['breakfast']['amount']=round($tda['meals']['breakfast']['value']*$tda['allowances']['24h']/100.0,2);
				}
				// Lunch
				if(!$tda['meals']['lunch']['amount'])
				{
					$tda['meals']['lunch']['amount']=round(($tda['allowances']['24h']-$tda['meals']['breakfast']['amount'])/2.00, 2);
					$tda['meals']['lunch']['value']=(100.0-$tda['meals']['breakfast']['value'])/2;
					$tda['meals']['lunch']['unit']='%';
				}
			}
			else
			{
				// Breakfast
				if(!$tda['meals']['breakfast']['amount'])
				{
					// Amount
					$tda['meals']['breakfast']['value']=get_double($GLOBALS['fxpglobals']['settings']['rk_frueh_inl']);
					$tda['meals']['breakfast']['amount']=$tda['meals']['breakfast']['value'];
				}
				// Lunch
				if(!$tda['meals']['lunch']['amount'])
				{
					// Percent
					$tda['meals']['lunch']['value']=get_double($GLOBALS['fxpglobals']['settings']['rk_mittagabend']);
					$tda['meals']['lunch']['unit']='%';
					// -> Amount
					$tda['meals']['lunch']['amount']=round($tda['meals']['lunch']['value']*$tda['allowances']['24h']/100.0,2);
				}
			}
			// Supper
			if(!$tda['meals']['supper']['amount'])
			{
				$tda['meals']['supper']['amount']=$tda['meals']['lunch']['amount'];
				$tda['meals']['supper']['value']=$tda['meals']['lunch']['value'];
				$tda['meals']['supper']['unit']=$tda['meals']['lunch']['unit'];
			}
//fxDebug($tda['meals'],'$tda[\'meals\']', 0);
		}

		$rsd=substr((string)$tda['sdate'],0,8);
		$red=substr((string)$tda['edate'],0,8);

		// Determine additional board expenses
		$ec=0;	// Emergency counter
		$disvset=true;
		if(strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['rk_8hpausch'],0,1)) == 'j')
			$disvset=false;
		$dsd=$rsd;
		while($dsd <= $red)
		{
			$year=(int)substr((string)$dsd,0,4);
			$month=(int)substr((string)$dsd,4,2);
			$day=(int)substr((string)$dsd,6,2);

			$wday=fx_date('w', fx_mktime(12,0,0, $month,$day,$year));

			$stime=0;
			if($dsd == $rsd)
				$stime=(int)substr((string)$tda['sdate'],8,2)*3600 + (int)substr((string)$tda['sdate'],10,2)*60 + (int)substr((string)$tda['sdate'],12,2);

			$etime=86400;
			if($dsd == $red)
				$etime=(int)substr((string)$tda['edate'],8,2)*3600 + (int)substr((string)$tda['edate'],10,2)*60 + (int)substr((string)$tda['edate'],12,2);

			$h=($etime-$stime)/3600;

			$tda['days']++;
			$tda['hours'] += $h;

			if($h >= $tda['maxh'])
			{
				$tda['maxh']=$h;
				$tda['maxd']=$dsd;
			}

			$tda['dates'][$dsd]=array('w'=>$wday, 'st'=>$stime, 'et'=>$etime, 'h'=>$h, 'dis'=>false, 'vs'=>true, 'dh'=>0.0);

			$dsd=fx_date('Ymd', fx_mktime(12,0,0, $month,$day+1,$year));
			$ec++;
			if($ec > 36500)	// Travel more than 100 years? Stop
				break;
		}

		// Calculate values
		foreach($tda['dates'] as $dsd => $da)
		{
			if($da['h'] >= 24.00)		// 24 hours
				$tda['dates'][$dsd]['dh']=$tda['allowances']['24h'];
			else if($rsd >= '20140101')	// since 01.01.2014
			{
				if(($tda['hours'] < 8.00) || (($tda['days'] < 3) && ($tda['hours'] < 10.0) && ($dsd != $tda['maxd'])))
					$tda['dates'][$dsd]['vs']=false;

				if($da['h'] > 8.00)		// > 8 & < 24 hours
				{
					$tda['dates'][$dsd]['dh']=$tda['allowances']['14h'];
					$tda['dates'][$dsd]['vs']=true;
				}
				else					// Arrival/Leave day
					$tda['dates'][$dsd]['dh']=$tda['allowances']['8h'];
			}
			else if($da['h'] >= 14.00)	// >= 14 & < 24 hours
				$tda['dates'][$dsd]['dh']=$tda['allowances']['14h'];
			else if($da['h'] >= 8.00)	// >= 8 & < 14 hours
				$tda['dates'][$dsd]['dh']=$tda['allowances']['8h'];
			else
				$tda['dates'][$dsd]['dh']=0.0;

			if($disvset && !$tda['dates'][$dsd]['vs'])
				$tda['dates'][$dsd]['dh']=0.0;

			$tda['dates'][$dsd]['dh']=max($tda['dates'][$dsd]['dh'], 0.0);
			if(!$tda['dates'][$dsd]['dh'])
				$tda['dates'][$dsd]['dis']=true;

			$tda['atot'] += $tda['dates'][$dsd]['dh'];

//echo('dsd: '.$dsd.' -- h: '.$da['h'].', dh: '.$tda['dates'][$dsd]['dh'].'<br />');
		}
	}

//fxDebug($tda,'$tda', 0);
	return $tda;
}

/**
 * ???FUNCTION???
 *
 * @param various $csv  - Mandatory parameter:                 ???PARAMETER???
 * @param integer $mlen - Optional parameter (default = 300):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function csvWrap($csv,$mlen=300)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ca=array();
	$csv=trim((string)$csv);
	while(strlen((string)$csv) > $mlen)
	{
		$mp=$mlen;
		while(substr((string)$csv,$mp,1) != ',')
			$mp--;
		$ca[]=trim(substr((string)$csv,0,$mp));
		$csv=trim(substr((string)$csv,$mp+1));
	}
	$ca[]=$csv;

	return $ca;
}

/**
 * ???FUNCTION???
 *
 * @param various $width     - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $with_nbsp - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dummySpace($width, $with_nbsp=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($with_nbsp)
		$nbsp='&nbsp;';
	else
		$nbsp='';
	return '<img src="'.$GLOBALS['gfxpath'].'dmy.png" border="0" width="'.$width.'" height="16" align="center">'.$nbsp;
}
?>