<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tools2.inc                                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2021-04-01                                                   //
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
 * @version 21.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Sonderfall: Profilname "Test-4.7.0" zum Testen der aktuellen fx-project Version (hier z.B. 4.7.0)
 */
function fxpTestLogAction()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>fxpTestLogAction:</b> tr='.$GLOBALS['fxpglobals']['tr'].', aktion='.$GLOBALS['fxpglobals']['action'].'<br>');
	if(($GLOBALS['fxpglobals']['tr'] >= 0) && ($GLOBALS['fxpglobals']['action'] >= 0))
	{
		$dac_unset = false;
		if(!isset($GLOBALS['dontAddClient']))
		{
			$dac_unset = true;
			$GLOBALS['dontAddClient'] = true;
		}

		$fxp_testprofil	= '[Test-'.$GLOBALS['version'].']';

		$sql  = "SELECT b.benutzergr_id FROM benutzergruppen b, wertetab w";
		$sql .= " WHERE w.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND w.id_feld=660 AND w.tabwert='".$fxp_testprofil."' AND b.gruppenname_id=w.wertetabid AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$fxp_bgid = (int)db_value($sql);
//fxDebug($fxp_bgid, $sql);

		if($fxp_bgid)
		{
			$sql = "SELECT rechte_trans_id, aktionscode FROM rechte_trans WHERE benutzergr_id=".$fxp_bgid." AND trans_id=".$GLOBALS['fxpglobals']['tr']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$fxp_rtid = db_values($sql);
//fxDebug($fxp_rtid, $sql);

			if(fxIsArray($fxp_rtid))
			{
				// Aktionscode mit aktueller Aktion anpassen
				$fxp_abit = array(1,2,4,8,16,32,64,128);
				$fxp_acode = ($fxp_rtid['aktionscode']&31)|$fxp_abit[$GLOBALS['fxpglobals']['action']];
				// Transaktionsaufrufcounter um 1 erhöhen
				$fxp_tcounter = (1+($fxp_rtid['aktionscode']>>5))<<5;

				$sql = "UPDATE rechte_trans SET aktionscode=".($fxp_tcounter|$fxp_acode)." WHERE benutzergr_id=".$fxp_bgid." AND trans_id=".$GLOBALS['fxpglobals']['tr']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				@db_x($sql, 0, false, false, false);	// Ohne Zeitstempeländerung und Historie
//fxDebug($sql, '$sql');
			}
		}

		if($dac_unset)
			unset($GLOBALS['dontAddClient']);
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $ka - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function keyGet($ka)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Schlüsseleingabe extrahieren
	if(fxIsArray($ka))
	{
		$k='';
		foreach($ka as $kt)
		{
			$kt=trim($kt);
			if(strlen($kt))
				$k=$kt;
		}
	}
	else
		$k=trim($ka);

	return $k;
}

/**
 * ______________
 * PROGRAMMKLASSE
 * --------------
 * 40 = Sysadmin
 * 30 = Admin1
 * 20 = Admin
 * 10 = User
 * 01 = Standard
 *
 * @param boolean $ret - Optional parameter (default = false): ???PARAMETER???
 * @param integer $c   - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $u   - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $p   - Optional parameter (default = 0):     ???PARAMETER???
 * @param string  $a   - Optional parameter (default = ''):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getPrgClass($ret=false,$c=0,$u=0,$p=0,$a='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$c=(int)$c;
	if(!$c && isset($GLOBALS['fxpglobals']['client']))
		$c=(int)$GLOBALS['fxpglobals']['client'];
	$u=(int)$u;
	if(!$u && isset($GLOBALS['fxpglobals']['user']))
		$u=(int)$GLOBALS['fxpglobals']['user'];
	$p=(int)$p;
	if(!$p && isset($GLOBALS['fxpglobals']['person']))
		$p=(int)$GLOBALS['fxpglobals']['person'];
	if(!strlen($a))
		$a=(int)$GLOBALS['fxpglobals']['useradmin'];

	$pc=1;	// Standard
	if($u)
	{
		if(($u == 1) || $a)
		{
			if(!$p)
			{
				$pc=40;	// Sysadmin
				if(!$ret)
					$GLOBALS['fxpglobals']['persdat']=array('personentyp'=>99, 'elter'=>0, 'abt_id'=>0, 'geschlecht'=>99);
			}
			else if($c == 1)
				$pc=30;	// Admin1
			else
				$pc=20;	// Admin
		}
		else
			$pc=10;	// User
	}

	if(!$ret)
		$GLOBALS['prgclass']=$pc;
	else
		return $pc;
}

/**
 * Adjust transaction stack and counter
 */
function trStack()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($GLOBALS['tr'] != 0)
	{
		$trp=$GLOBALS['tr'];
		if(isset($_GET['repid']))
			$trp .= '_repid='.$_GET['repid'];

		// Has transaction changed
		$n=true;
		if(fxIsArray($GLOBALS['fxpglobals']['trstack']) && ($GLOBALS['fxpglobals']['trstack'][0] == $trp))
			$n=false;

		// Stack
		$a=array(0=>$trp);
		if(fxIsArray($GLOBALS['fxpglobals']['trstack']))
		{
			$c=1;
			foreach($GLOBALS['fxpglobals']['trstack'] as $str)
			{
				if($str !== $trp)
				{
					$a[$c]=$str;
					$c++;
				}
			}
		}
		$GLOBALS['fxpglobals']['trstack']=$a;
//fxDebug($GLOBALS['fxpglobals']['trstack'], 'trstack');

		// Counter
		if(!isset($GLOBALS['fxpglobals']['trcounter']))
			$GLOBALS['fxpglobals']['trcounter']=array($trp=>1);
		else if(!isset($GLOBALS['fxpglobals']['trcounter'][$trp]))
			$GLOBALS['fxpglobals']['trcounter'][$trp]=1;
		else if($n)
			$GLOBALS['fxpglobals']['trcounter'][$trp]++;
//fxDebug($GLOBALS['fxpglobals']['trcounter'], 'trcounter');
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $fieldname - Mandatory parameter:                    ???PARAMETER???
 * @param various $message   - Mandatory parameter:                    ???PARAMETER???
 * @param integer $code      - Optional parameter (default = 300000):  ???PARAMETER???
 * @param integer $mnr       - Optional parameter (default = 0):       ???PARAMETER???
 * @param various $use_id    - Optional parameter (default = -1):      ???PARAMETER???
 */
function addFieldMessage($fieldname,$message,$code=300000,$mnr=0,$use_id=-1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!fxIsArray($fieldname))
		$fieldarray=array($fieldname);
	else
		$fieldarray=$fieldname;

	foreach($fieldarray as $fieldname)
	{
		if($use_id < 0)
		{
			if(!is_array($GLOBALS['fxpvars']['msgarr']))
				$GLOBALS['fxpvars']['msgarr']=array();
			if(!is_array($GLOBALS['fxpvars']['msgarr'][$code]))
			{
				$GLOBALS['fxpvars']['msgarr'][$code]=array();
				$k=0;
			}
			else
				$k=sizeof($GLOBALS['fxpvars']['msgarr'][$code]);
			$id='M'.$k;
			$GLOBALS['fxpvars']['msgarr'][$code][$k]=array('nr'=>$id,'val'=>$message);
		}
		else
			$id='M'.$use_id;

		if(!is_array($GLOBALS['fxpvars']['lookups']))
			$GLOBALS['fxpvars']['lookups']=array();
		if(!is_array($GLOBALS['fxpvars']['lookups'][$mnr]))
			$GLOBALS['fxpvars']['lookups'][$mnr]=array();
		$GLOBALS['fxpvars']['lookups'][$mnr][$fieldname]=array('position'=>$code,'id'=>$id,'tabwert'=>$message);
	}
}

/**
 * Create sql's for creating default profiles from a text file
 *
 * @param integer $cid - Optional parameter (default = 1):  Client number
 *
 * @return Array containing sql's for creating default profiles
 */
function fxf_getDefaultProfiles($cid=1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cid=(int)$cid;
	$pcalls=array();

	// All available languages
	$lar=getLanguages(true);
//fxDebug($lar,'$lar', 0);

	// Load default profiles text
	$filename=$GLOBALS['datpath'].'profiles.txt';
	$fca=fxLoad($filename, 1);
//fxDebug($fca,'$fca: $filename='.$filename, 0);
	if(fxIsArray($fca))
	{
		$db_zeitstempel="'".$GLOBALS['datetime']."'";
		$fl=1;
		$sc=0;
		foreach($fca as $fline)
		{
			$gte=explode('|', trim($fline));
			if(fxIsArray($gte) && (sizeof($gte) == 2))
			{
				$ge=explode('^', $gte[0]);
				$te=explode(',', $gte[1]);
				if(fxIsArray($ge))
				{
					// Get new id's for profile group and name
					$pg_id=id_max('benutzergruppen', 'benutzergr_id', '', '', 100);
					$pg_name_id=id_max('wertetab', 'wertetabid', '', $cid);

					// Create profile group in table "benutzergruppen"
					$sql  = "INSERT INTO benutzergruppen";
					$sql .= " (benutzergr_id, mandanten_id, gruppenname_id, transid, aenderungs_id, zeitstempel) ";
					$sql .= "VALUES";
					$sql .= " (".$pg_id.", ".$cid.", ".$pg_name_id.", ".(int)$GLOBALS['fxpglobals']['tr'].", ".$GLOBALS['fxpglobals']['person'].", ".$db_zeitstempel.")";
					$pcalls[$sc++]=$sql;

					// Create profile names in table "wertetab"
					$dpn='Profile #'.$fl;
					$pna=array();
					foreach($ge as $gl)
					{
						$ep=strpos($gl, '=');
						if($ep !== false)
						{
							$lang=(int)substr($gl,0,$ep);
							$name=trim(substr($gl,$ep+1));
							$pna[$lang]=$name;
							if(($lang == $GLOBALS['fxpglobals']['lang']) && strlen($name))
								$dpn=$name;
						}
					}
//fxDebug($pna,'$pna: $dpn='.$dpn, 0);
					foreach($lar as $lang => $lname)
					{
						if(isset($pna[$lang]))
							$name=$pna[$lang];
						else if($lang == $GLOBALS['fxpglobals']['lang'])
							$name=$dpn;
						else
							$name='§'.$dpn;

						$sql  = "INSERT INTO wertetab";
						$sql .= " (mandanten_id, wertetabid, id_feld, id_sprache, position, tabwert, satzvers, archiv, transid, aenderungs_id, zeitstempel) ";
						$sql .= "VALUES";
						$sql .= " (".$cid.", ".$pg_name_id.", 660, ".$lang.", 0, '".convert_string($name, 'todb')."', 0, 0, ".(int)$GLOBALS['fxpglobals']['tr'].", ".$GLOBALS['fxpglobals']['person'].", ".$db_zeitstempel.")";
						$pcalls[$sc++]=$sql;
					}

					// Assign program functions to profile group in table "rechte_trans"
					if(fxIsArray($te))
					{
						foreach($te as $tl)
						{
							$mp=strpos($tl, '-');
							if($mp !== false)
							{
								$atr=(int)substr($tl, 0, $mp);
								$tcd=(int)substr($tl, $mp+1);
							}
							else
							{
								$atr=(int)$tl;
								$tcd=30;
							}

							// Get new id for program function reference
							$pg_tr_id=id_max('rechte_trans', 'rechte_trans_id');

							$sql  = "INSERT INTO rechte_trans";
							$sql .= " (mandanten_id, rechte_trans_id, trans_id, benutzergr_id, aktionscode, transid, aenderungs_id, zeitstempel) ";
							$sql .= "VALUES";
							$sql .= " (".$cid.", ".$pg_tr_id.", ".$atr.", ".$pg_id.", ".$tcd.", ".(int)$GLOBALS['fxpglobals']['tr'].", ".$GLOBALS['fxpglobals']['person'].", ".$db_zeitstempel.")";
							$pcalls[$sc++]=$sql;
						}
					}

					$fl++;
				}
			}
		}
	}

	return $pcalls;
}

/**
 * ???FUNCTION???
 *
 * @param string $did   - Optional parameter (default = ''): ???PARAMETER???
 * @param string $title - Optional parameter (default = ''): ???PARAMETER???
 * @param string $sign  - Optional parameter (default = ''): ???PARAMETER???
 */
function drawPrint($did='', $title='', $sign='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$tt=meldung(967);	// Print
	if(strlen($title))
		$tt .= ' &quot;'.$title.'&quot;';
	$icon .= '<span style="position:relative;margin:0;padding:0;cursor:pointer;" tooltip="'.$tt.'" '.fxf_jsFunction('print', $did, $title).'><img src="'.$GLOBALS['gfxpath'].'im_print.png" style="margin:0;padding:0;">';
	if(strlen($sign))
		$icon .= '<div style="position:absolute;left:7px;top:-11px;border:1px solid #888888;border-top-left-radius:4px;border-top-right-radius:4px;padding:0 2px;background:#eeeeee;color:#000000;font-size:6pt;font-weight:bold;text-shadow:none;cursor:pointer;" tooltip="'.$tt.'" '.fxf_jsFunction('print', $did, $title).'>'.$sign.'</div>';
	$icon .= '</span>';

	$ibp=-1;
	while(isset($GLOBALS['fxpvars']['ibuffer'][$ibp]))
		$ibp--;
	$GLOBALS['fxpvars']['ibuffer'][$ibp]=$icon;
}

/**
 * ???FUNCTION???
 *
 * @param integer $cuid - Optional parameter (default = 0): ???PARAMETER???
 * @param integer $clid - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getCurrency($cuid=0, $clid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$clid=(int)$clid;
	if(!$clid)
		$clid=$GLOBALS['fxpglobals']['client'];

	$cuid=(int)$cuid;
	if(!$cuid)
	{
		$sql="SELECT einstellungswert FROM s_einstellungswerte WHERE einstellungs_id=15 AND mandanten_id=".$clid;
		$cuid=(int)db_value($sql);
		if(!$cuid)
		{
			$sql="SELECT einstellungswert FROM s_einstellungswerte WHERE einstellungs_id=15 AND mandanten_id=0";
			$cuid=(int)db_value($sql);
		}
	}

	if(!isset($GLOBALS['client_currencies']))
		$GLOBALS['client_currencies']=array();
	if(!isset($GLOBALS['client_currencies'][0]))
		$GLOBALS['client_currencies'][0]=$cuid;
	if(!isset($GLOBALS['client_currencies'][$cuid]))
		$GLOBALS['client_currencies'][$cuid]=meldung($cuid,false);

	return $GLOBALS['client_currencies'][$cuid];
}

/**
 * validate_entry
 * Diese Funktion validiert die vom Benutzer gemachten Einträge, d.h. sie wandelt die Einträge
 * entspr. des angegebenen Typs in das dafür gewünschte Datenbankformat um.
 * ----------------------------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $entry:		String, der überprüft werden soll
 * $err:		Variable, die im Fehlerfall die Fehlermeldung enthält
 * $feldarray:	<optional>	Feldtyp, der überprüft werden soll als string (Standard: 'text'), oder
 * Maskenfeldarray, dass die nötigen Infos enthält wie feldtyp, muss, nachkomma
 * $muss:		<optional>	'1', falls Eingabe erforderlich (Standard: '0')
 * $nachkomma:	<optional>	Anzahl der Nachkommastellen bei Feldtyp 'dezimal' oder 'betrag' (Standard: '2')
 * ----------------------------------------------------------------------------------------------
 * Rückgabe:
 * °°°°°°°°°
 * - Falls $err == '':	Neuer für die Datenbank konvertierter Wert
 * - Falls $err != '':	Alter Wert im Fehlerfall, dann enthält $err die Fehlermeldung
 * ----------------------------------------------------------------------------------------------
 * Beispiel:
 * °°°°°°°°°
 * $err='';
 * $neu=validate_entry('120301', $err, 'datum');
 * -> liefert als Ergebnis zurück: '20010312000000'
 * ----------------------------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $entry        - Mandatory parameter:                    ???PARAMETER???
 * @param various &$err         - Mandatory parameter:                    ???PARAMETER???
 * @param string  $feldarray    - Optional parameter (default = 'text'):  ???PARAMETER???
 * @param string  $muss         - Optional parameter (default = '0'):     ???PARAMETER???
 * @param string  $nachkomma    - Optional parameter (default = '2'):     ???PARAMETER???
 * @param string  $trennzeichen - Optional parameter (default = ''):      ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validate_entry($entry, &$err, $feldarray='text', $muss='0', $nachkomma='2', $trennzeichen='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $debug;

	$GLOBALS['error_msg_array_vars']=array();

	if(fxIsArray($feldarray))
	{
		$typ=$GLOBALS['_ftypes'][$feldarray['feldtyp']];
		$fnm=$feldarray['feldname'].'_muss';
		if(isset($GLOBALS[$fnm]))
			$muss=(string) $GLOBALS[$fnm];
		else
			$muss=(string) $feldarray['muss'];
		$nachkomma=(int) $feldarray['nachkomma'];
	}
	else if((string)((int)$feldarray) == (string)$feldarray)
		$typ=$GLOBALS['_ftypes'][(int)$feldarray];
	else
		$typ=strtolower($feldarray);

	$err=0;
	$rs='';
	$pentry=(string)$entry;
	if(strlen($pentry) && ($typ != 'text') && ($typ != 'memo') && ($typ != 'systemvariable') && ($typ != 'text4000'))
		$entry=removeTags($pentry, true);
	else
		$entry=$pentry;

	if($debug)
		echo('<b>validate_entry</b> $typ='.$typ.', $muss='.$muss.', $pentry=['.fxHtmlEncode($pentry).'], $entry=['.fxHtmlEncode($entry).']<br />');

	if($muss != '0') // MUSS-Feld
	{
		// Entry anhand des Feldtyps auf Fehler auswerten
		switch($typ)
		{
			case 'text':
			case 'memo':
			case 'systemvariable':
			case 'text4000':
				if(!strlen($entry))
					$err=749; // Feld darf nicht leer sein. Bitte Text eingeben!
			break;

			case 'yes/no':
			case 'ja/nein':
			case 'checkbox':
				$entry=substr((string)((int)$entry), 0, 1);
				if($entry != '1')
					$err=750; // Feld muss gesetzt sein!
			break;

			case 'integer':
			case 'ganzzahl':
			case 'longinteger':
			case 'ganzzahllang':
			case 'decimal':
			case 'dezimal':
			case 'amount':
			case 'betrag':
			case 'budget_int':
			case 'budget_ext':
				$dentry=get_double($entry, $GLOBALS['fxpglobals']['settings']['deztrennzeichen']);
				if(($muss == '2') && (!strlen($entry) || ($dentry == 0.0) || ($dentry < 0.0)))
					$err=596; // Das Feld $var1 darf nicht leer sein. Bitte geben Sie eine gültige, positive Zahl ungleich 0 oder 0,00 ein!
				else if(($muss == '-2') && (!strlen($entry) || ($dentry == 0.0)))
					$err=626; // Das Feld $var1 darf nicht leer sein. Bitte geben Sie eine gültige Zahl ungleich 0 oder 0,00 ein!
				else if(($muss == '1') && (!strlen($entry) || ($dentry < 0.0)))
					$err=3339; // Das Feld $var1 darf nicht leer sein. Bitte geben Sie eine gültige, positive Zahl oder 0 oder 0,00 ein!
				else if(($muss != '3') && !strlen($entry))
					$err=751; // Das Feld $var1 darf nicht leer sein. Bitte Zahl eingeben!
			break;

			case 'date':
			case 'datum':
				if(!strlen($entry))
					$err=752; // Feld darf nicht leer sein. Bitte Datum eingeben!
			break;

			case 'time':
			case 'zeit':
				if(!strlen($entry))
					$err=753; // Feld darf nicht leer sein. Bitte Zeit eingeben!
			break;

			case 'timespan':
			case 'zeitspanne':
			case 'timespanm':
			case 'zeitspannem':
			case 'timespant':
			case 'zeitspannet':
			case 'timespani':
			case 'zeitspannei':
				if(!strlen($entry))
					$err=885; // Feld darf nicht leer sein. Bitte Zeitspanne eingeben!
			break;

			case 'select':
			case 'auswahl':
				if(!$entry)
					$err=734;	// Bitte wählen sie im Feld "$var1" einen Wert aus.
			break;

			default:
				if(!strlen($entry))
					$err=754; // Feld darf nicht leer sein!
			break;
		}
	}

	// Entry anhand des Feldtyps auswerten, allgemein
	if(strlen($entry))
	{
		$entry=convert_string($entry, 'fromdb');
		switch($typ)
		{
			case 'yes/no':
			case 'ja/nein':
			case 'checkbox':
				$entry=substr((string)((int)$entry), 0, 1);
				if($entry != '1')
					$rs='0';
				else
					$rs='1';
			break;

			case 'integer':
			case 'ganzzahl':
			case 'longinteger':
			case 'ganzzahllang':
			case 'decimal':
			case 'dezimal':
			case 'amount':
			case 'betrag':
			case 'budget_int':
			case 'budget_ext':
				$tentry	= '';
				$bfound	= false;
				$pos	= -1;
				for($spos=0; $spos<strlen($entry); $spos++) // nach erster Zahl suchen
				{
					$cz=substr($entry, $spos, 1);
					if($cz == ' ')
						continue;
					else if(is_numeric($cz))
					{
						$tentry .= $cz;
						if($pos < 0)
							$pos=$spos;
					}
					else if(($cz != ',') && ($cz != '.') && ($cz != '-') && ($cz != '+'))
						$bfound=true;
					else if($cz != '+')
						$tentry .= $cz;
				}
				if(($pos < 0) || $bfound) // Falsche Eingabe
				{
					$err=755; // Bitte Zahl eingeben
					break;
				}
				$zahl=get_double($tentry, $GLOBALS['fxpglobals']['settings']['deztrennzeichen']);
				if((($muss == '-1') && ($zahl <= 0.0)) || ((($muss == '1') || ($muss == '2') || ($muss == '3')) && ($zahl < 0.0)))
				{
					$err=1524; // Bitte geben Sie in das Feld "$var1" eine gültige, positive Zahl ein!
					break;
				}
				if(($typ == 'integer') || ($typ == 'ganzzahl') || ($typ == 'longinteger') || ($typ == 'ganzzahllang'))
					$rs=(string)round($zahl, 0);
				else
					$rs .= (string)(number_format(round($zahl, $nachkomma), $nachkomma, '.', ''));
			break;

			case 'email':
				$atpos=strpos($entry, '@');
				if($atpos > 0)
				{
					$sppos=strpos($entry, ' ');
					$ptpos=strpos($entry, '.', $atpos+1);
					if($sppos || ($ptpos <= $atpos) || ($ptpos == strlen($entry)-1))
					{
						$err=1400; // Keine gültige E-Mail-Adresse! Bitte korrigieren.
						break;
					}
					else
						$rs=$entry;
				}
				else
					$err=1400;
			break;

			case 'date':
			case 'datum':
				$msg=validate_date($entry, $err);
				if($msg !== false)
					$rs=$msg.'000000';
			break;

			case 'time':
			case 'zeit':
				$msg=validate_time($entry, $err);
				if($msg !== false)
					$rs='00000000'.$msg;
			break;

			case 'datetime':
				$entries=explode(' ', $entry, 2);
				$msg=validate_date($entries[0], $err);
				if($msg !== false)
				{
					$rs=$msg;
					$msg=validate_time($entries[1], $err);
					if($msg !== false)
						$rs .= $msg;
				}
			break;

			case 'userformat':
				$msg=validate_userformat($entry, $err, $trennzeichen);
				if($msg !== false)
					$rs=$msg;
			break;

			case 'timespan':
			case 'zeitspanne':
			case 'timespanm':
			case 'zeitspannem':
			case 'timespant':
			case 'zeitspannet':
			case 'timespani':
			case 'zeitspannei':
				$pos=-1;
				$neg='';
				for($spos=0; $spos<strlen($entry); $spos++) // nach erster Zahl suchen
				{
					$ezp=substr($entry,$spos,1);
					if($ezp == '-')
						$neg='-';
					else if(is_numeric($ezp))
					{
						$pos=$spos;
						break;
					}
				}
				if($pos < 0) // Falsche Eingabe
				{
					$err=886; // Bitte Zeitspanne eingeben
					break;
				}

				$zahl=''; $dez=false; $einheit=''; $umwandeln=true;
				for($zpos=$pos; $zpos<strlen($entry); $zpos++) // Zahl + Einheit extrahieren
				{
					$ezp=substr($entry, $zpos, 1);
					if(is_numeric($ezp))
						$zahl .= $ezp;
					else if(($ezp == ',') || ($ezp == '.'))
					{
						$zahl .= '.';
						$dez=true;
					}
					else if(($ezp == ':') || ($ezp == ' '))
					{
						$umwandeln=false;
						break;
					}
					else if(ord($ezp) != 0x20)
						$einheit .= strtolower($ezp);
				}
//echo('$typ=<b>'.$typ.'</b>: $entry='.fxHtmlEncode($entry).', $zahl='.fxHtmlEncode($zahl).', $einheit='.fxHtmlEncode($einheit).' - $umwandeln='.$umwandeln.'<br />');

				$msg=false;
				if($umwandeln)
				{
					if(isset($GLOBALS['Zeiteinheit']))
					{
						$z_name='Zeiteinheit';
						$z_id=330;
						if(!$dez && (($typ == 'timespanm') || ($typ == 'zeitspannem')))
							$z_pre=856;
						else
							$z_pre=484;
						$zanz=array(-3 => array('s', 6), 856 => array('m', 5), 484 => array('h', 4), 485 => array('mt', 3), 923 => array('mw', 2), 486 => array('mm', 1), 487 => array('mj', 0));
					}
					else
					{
						$z_name='zeitanzeige';
						$z_id=733;
						if(!$dez && (($typ == 'timespanm') || ($typ == 'zeitspannem')))
							$z_pre=-2;
						else
							$z_pre=916;
						$zanz=array(-3 => array('s', 6), -2 => array('m', 5), 916 => array('h', 4), 917 => array('mt', 3), -1 => array('mw', 2), 918 => array('mm', 1), 919 => array('mj', 0));
					}

					$zs=array
					(
						array($GLOBALS['fxpglobals']['settings']['mjahr']*$GLOBALS['fxpglobals']['settings']['mtag']*3600, $GLOBALS['fxpglobals']['settings']['mmonat']*$GLOBALS['fxpglobals']['settings']['mtag']*3600, $GLOBALS['fxpglobals']['settings']['mwoche']*$GLOBALS['fxpglobals']['settings']['mtag']*3600, $GLOBALS['fxpglobals']['settings']['mtag']*3600, 3600, 60, 1),
						array(1, $GLOBALS['fxpglobals']['settings']['mjahr']/$GLOBALS['fxpglobals']['settings']['mmonat'], $GLOBALS['fxpglobals']['settings']['mmonat']/$GLOBALS['fxpglobals']['settings']['mwoche'], $GLOBALS['fxpglobals']['settings']['mwoche'], $GLOBALS['fxpglobals']['settings']['mtag'], 60, 60)
					);

					if(!isset($GLOBALS[$z_name]) || ($GLOBALS[$z_name] == ''))
						$z=$z_pre;
					else
						$z=(int)$GLOBALS[$z_name];

					if($z == 0)
						$z=$z_pre;

					$zeiteh_array=db_values("SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=".$z_id." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');
					for($i=0; $i<sizeof($zeiteh_array); $i++)
						$zeiteh_array[$i]['value']=strtolower($zeiteh_array[$i]['tabwert']);
					$found=-1;
					if($einheit)
					{
						// Prüfen, ob gültige Einheit
						while($einheit && ($found < 0))
						{
							for($i=0; $i<sizeof($zeiteh_array); $i++)
							{
								if($einheit == substr($zeiteh_array[$i]['value'], 0, strlen($einheit)))
								{
									$found=$i;
									break;
								}
							}
							$einheit=substr($einheit, 0, -1);
						}
					}

					if($found >= 0)
						$z=$zeiteh_array[$found]['wertetabid'];

					$sek='0:00:'.(string)($zahl * $zs[0][$zanz[$z][1]]);
//echo('$found='.$found.', $einheit='.fxHtmlEncode($einheit).', $z='.$z.' - $sek='.$sek.'<br />');

					$msg=validate_time($sek, $err, $typ);
					if($msg !== false)
						$rs=$neg.$msg;
				}
				else
				{
					$msg=validate_time($entry, $err, $typ);
					if($msg !== false)
						$rs=$neg.$msg;
				}
//echo('$msg='.fxHtmlEncode($msg).'<hr />');

				if(($muss == '2') && (strlen($neg) || (($msg !== false) && !$msg)))
					$err=2419;	// Bitte geben Sie in das Feld "$var1" eine gültige, positive Zeitspanne ungleich 0 ein!
			break;

			default:
				$rs=$entry;
			break;
		}
	}

	if(!$err)
		return $rs;
	if($pentry)
		return '^'.$pentry;
	return $pentry;
}

/**
 * validate_date
 * Unterfunktion zu validate_entry: Versucht eine Benutzereingabe in das Datumsformat der
 * Datenbank zu konvertieren in der Form: 'yyyymmdd'
 * ----------------------------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $entry:		String, der überprüft werden soll
 * $err:		Variable, die im Fehlerfall die Fehlermeldung enthält
 * ----------------------------------------------------------------------------------------------
 * Rückgabe:
 * °°°°°°°°°
 * - false	im Fehlerfall, dann enthält $err die Fehlermeldung, sonst
 * - Konvertierter Wert
 * ----------------------------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $entry     - Mandatory parameter:                   ???PARAMETER???
 * @param various &$err      - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $ret_value - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validate_date($entry, &$err, $ret_value=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$trz='';
	$cd=strtolower($GLOBALS['fxpglobals']['settings']['formatdatum']);
	for($spos=0; $spos<strlen($cd); $spos++) // nach ersten Trennzeichen suchen
	{
		$cz=substr($cd,$spos,1);
		if(($cz == '.') || ($cz == ',') || ($cz == '-') || ($cz == '/'))
		{
			$trz=$cz;
			break;
		}
	}

	$entry=removeTags($entry, true);
	if(strlen($entry) && strlen($trz))
		$entry=str_replace(' ', $trz, $entry);
	$pos=-1;
	$bfound=false;
	for($spos=0; $spos<strlen($entry); $spos++) // nach erster Zahl suchen
	{
		$cz=substr($entry,$spos,1);
		if(is_numeric($cz))
		{
			if($pos < 0)
				$pos=$spos;
		}
		// ',' erlaubt, da datetime Format dieses Zeichen enthält
		else if(($cz != '.') && ($cz != ',') && ($cz != '-') && ($cz != '/'))
			$bfound=true;
	}
	if(($pos < 0) || $bfound) // Falsche Eingabe
	{
		$err=756; // Bitte Datum eingeben!
		return false;
	}

	// Logikprüfung: Falls $entry bereits im Datumsformat vorliegt, d.h. falls es nur aus Zahlen besteht und eine bestimmte Länge hat, dann gleich zurückliefern ohne erneute Formatierung
	$sle=strlen($entry);
	if(($sle == 8) || ($sle == 14))
	{
		$no_number=0;
		for($i=0; $i<$sle; $i++)
		{
			if(!is_numeric(substr($entry, $i, 1)))
				$no_number++;
		}
		if(!$no_number)
		{
//echo('<hr>validate_date<i>('.$entry.')</i><br />-&gt; <b style="color:red;">'.substr($entry, 0, 8).'</b><br />');
			return substr($entry, 0, 8);
		}
	}

	$tmp=array();
	$tz=0;
	for($zpos=$pos; $zpos<strlen($entry); $zpos++) // Zahlen extrahieren
	{
		if(is_numeric(substr($entry, $zpos, 1)))
			$tmp[$tz] .= substr($entry, $zpos, 1);
		else
		{
			$tz++;
			while(($zpos < strlen($entry)) && (!is_numeric(substr($entry, $zpos, 1))))
				$zpos++;
			if($zpos < strlen($entry))
				--$zpos;
		}
	}
	// Benutzerinfos ermitteln bzgl. Datum
	$tag=0; $monat=1; $jahr=2;
	$pos=0;
	for($i=0; $i<strlen($cd); $i++)
	{
		switch($cd[$i])
		{
			case 'y':
				$jahr=$pos;
				break;
			case 'm':
				$monat=$pos;
				break;
			case 'd':
				$tag=$pos;
				break;
			default:
				$pos++;
		}
	}
	// Datum in 3 Teile aufsplitten -> Tag, Monat, Jahr
	switch(sizeof($tmp))
	{
		case 1: // Nur ein Wert angegeben
			$len=strlen($tmp[0]);
			if($len == 6)
			{
				$datum[0]=substr($tmp[0], $tag*2, 2);
				$datum[1]=substr($tmp[0], $monat*2, 2);
				$datum[2]=substr($tmp[0], $jahr*2, 2);
			}
			else if($len == 8)
			{
				if($jahr == 0)
				{
					$datum[0]=substr($tmp[0], 2+$tag*2, 2);
					$datum[1]=substr($tmp[0], 2+$monat*2, 2);
					$datum[2]=substr($tmp[0], 0, 4);
				}
				else
				{
					$datum[0]=substr($tmp[0], $tag*2, 2);
					$datum[1]=substr($tmp[0], $monat*2, 2);
					$datum[2]=substr($tmp[0], 4, 4);
				}
			}
			else
			{
				$err=758;
				return false;
			}
			break;

		case 2: // Zwei Werte angegeben, ausgegangen wird von Tag + Monat
			if($jahr == 0)
			{
				$datum[0]=$tmp[$tag-1];
				$datum[1]=$tmp[$monat-1];
			}
			else
			{
				$datum[0]=$tmp[$tag];
				$datum[1]=$tmp[$monat];
			}
			$datum[2]=fx_date('Y');
			break;

		default: // alle Werte angegeben
			$datum[0]=$tmp[$tag];
			$datum[1]=$tmp[$monat];
			$datum[2]=$tmp[$jahr];
			break;
	}
	// Jahr umwandeln
	if(strlen($datum[2]) == 1)
		$datum[2]=(string) (2000 + (int) $datum[2]);
	else if(strlen($datum[2]) == 2)
	{
		if((int) $datum[2] < (int) $GLOBALS['fxpglobals']['settings']['jhwechsel'])
			$datum[2]=substr(fx_date('Y'), 0, 2).$datum[2];
		else
			$datum[2]=(string)((int)substr(fx_date('Y'), 0, 2) - 1).$datum[2];
	}

	if(!$GLOBALS['db_update'])
	{
		// Monat muss zwischen 1 und 12 liegen
		$datum[1]=MAX(1, MIN(12, (int)$datum[1]));

		// Tag muss zwischen 1 und "Monatstage" liegen
		$mo_tage=array(1 => 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31);
		if(strlen($datum[2]) == 4)
			$lj=fx_is_leap_year($datum[2]);
		else
			$lj=false;
		if($lj)	// Schaltjahr
			$mo_tage[2]=29;
		$datum[0]=MAX(1, MIN($mo_tage[$datum[1]], (int)$datum[0]));
	}

	if(checkdate($datum[1], $datum[0], $datum[2]) == false)
	{
		$err=758;
		if($ret_value)
		{
			$datum[0]=substr('00'.$datum[0], -2);
			$datum[1]=substr('00'.$datum[1], -2);
			$datum[2]=substr('0000'.$datum[2], -4);
//echo('<hr>validate_date<i>('.$entry.')</i><br />-&gt; <b style="color:green;">'.$datum[2].$datum[1].$datum[0].'</b><br />');
			return($datum[2].$datum[1].$datum[0]);
		}
		else
			return false;
	}

	// Prüfen, ob Datum in den erlaubten Grenzen
	if(!$GLOBALS['fxptdata']['ignore_date_boundaries'])
	{
		setDateLimits();
		if(($datum[2] < $GLOBALS['fxpvars']['date_minmax']['min']) || ($datum[2] > $GLOBALS['fxpvars']['date_minmax']['max']))
		{
			if($datum[2] < $GLOBALS['fxpvars']['date_minmax']['min'])
			{
				$err=1901;	// Im Feld "$var1" darf das Datum nicht vor dem Jahr [$var2] liegen!
				$GLOBALS['error_msg_array_vars'][2]=$GLOBALS['fxpvars']['date_minmax']['min'];
			}
			else
			{
				$err=1902;	// Im Feld "$var1" darf das Datum nicht nach dem Jahr [$var2] liegen!
				$GLOBALS['error_msg_array_vars'][2]=$GLOBALS['fxpvars']['date_minmax']['max'];
			}

			if($ret_value)
			{
//echo('<hr>validate_date<i>('.$entry.')</i><br />-&gt; <b style="color:green;">'.substr('0000'.$datum[2], -4).substr('00'.$datum[1], -2).substr('00'.$datum[0], -2).'</b><br />');
				return(substr('0000'.$datum[2], -4).substr('00'.$datum[1], -2).substr('00'.$datum[0], -2));
			}
			else
				return false;
		}
	}

	// alles ok, dann in korrektes Format umwandeln
//echo('<hr>validate_date<i>('.$entry.')</i><br />-&gt; <b style="color:blue;">'.substr('0000'.$datum[2], -4).substr('00'.$datum[1], -2).substr('00'.$datum[0], -2).'</b><br />');
	return(substr('0000'.$datum[2], -4).substr('00'.$datum[1], -2).substr('00'.$datum[0], -2));
}

/**
 * validate_time
 * Unterfunktion zu validate_entry: Versucht eine Benutzereingabe in das Zeitformat der
 * Datenbank zu konvertieren in der Form: 'hhnnss'
 * ----------------------------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $entry:		String, der überprüft werden soll
 * $err:		Variable, die im Fehlerfall die Fehlermeldung enthält
 * ----------------------------------------------------------------------------------------------
 * Rückgabe:
 * °°°°°°°°°
 * - false	im Fehlerfall, dann enthält $err die Fehlermeldung, sonst
 * - Konvertierter Wert
 * ----------------------------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $entry - Mandatory parameter:                    ???PARAMETER???
 * @param various &$err  - Mandatory parameter:                    ???PARAMETER???
 * @param string  $typ   - Optional parameter (default = 'zeit'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validate_time($entry, &$err, $typ='zeit')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$entry=removeTags($entry, true);
	$pos=-1;
	for($spos=0; $spos<strlen($entry); $spos++) // nach erster Zahl suchen
	{
		$ezp=substr($entry,$spos,1);
		if(is_numeric($ezp))
		{
			$pos=$spos;
			break;
		}
	}
	if($pos < 0) // Falsche Eingabe
	{
		if($typ == 'zeit')
			$err=757; // Bitte Uhrzeit eingeben!
		else
			$err=886; // Bitte Zeitspanne eingeben!
		return false;
	}

	$dtz=$GLOBALS['fxpglobals']['settings']['deztrennzeichen'];	// ,
	if($dtz == ',')
		$ttz='.';
	else
		$ttz=',';
	$tmp=array(); $tz=0; $cp=-1;
	for($zpos=$pos; $zpos<strlen($entry); $zpos++) // Zahlen extrahieren
	{
		$cc=substr($entry,$zpos,1);
		if(is_numeric($cc))
			$tmp[$tz] .= $cc;
		else if($cc != $ttz)
		{
			$tz++;
			if($cc == $dtz)
				$cp=$tz;
			while(($zpos < strlen($entry)) && (!is_numeric($cc)))
			{
				$zpos++;
				$cc=substr($entry, $zpos, 1);
			}
			if($zpos < strlen($entry))
				--$zpos;
		}
	}
//fxDebug($tmp,'$tmp: $entry='.fxHtmlEncode($entry));
	// Kommazeit, z.B. 2,25 => 2:15
	if($cp >= 0)
		$tmp[$cp]=ceil($tmp[$cp]*60/100);
	// Benutzerinfos ermitteln bzgl. Zeit
	$stunde=0; $minute=1; $sekunde=2;
	$pos=0;
	$cd=strtolower($GLOBALS['fxpglobals']['settings']['formatzeit']);
	for($i=0; $i<strlen($cd); $i++)
	{
		switch($cd[$i])
		{
			case 'h':
				$stunde=$pos;
			break;
			case 'n':
				$minute=$pos;
			break;
			case 's':
				$sekunde=$pos;
			break;
			default:
				$pos++;
		}
	}
	// Zeit in 3 Teile aufsplitten -> Stunde, Minute, Sekunde
	switch(sizeof($tmp))
	{
		case 1: // Nur ein Wert angegeben
			if($typ == 'zeit')
			{
				$len=strlen($tmp[0]);
				if($len > 3)
				{
					$zeit[0]=substr($tmp[0], 0, 2);
					$zeit[1]=substr($tmp[0], 2, 2);
					if($len > 4)
						$zeit[2]=substr($tmp[0], 4, 2);
					else
						$zeit[2]='0';
				}
				else
				{
					if((int) $tmp[0] < 25)
					{
						$zeit[0]=$tmp[0];
						$zeit[1]='0';
					}
					else
					{
						$zeit[0]=(string) (((int) $tmp[0]) / 60);
						$zeit[1]=(string) (((int) $tmp[0]) % 60);
					}
					$zeit[2]='0';
				}
			}
			else
			{
				$zeit[0]='0';
				$zeit[1]=$tmp[0];
				$zeit[2]='0';
			}
		break;

		case 2: // Zwei Werte angegeben
			if(strpos($cd, 'h') !== false) // Stunden + Minuten???
			{
				$zeit[0]=$tmp[$stunde];
				$zeit[1]=$tmp[$minute];
				$zeit[2]='0';
			}
			else // Minuten + Sekunden???
			{
				$zeit[0]='0';
				$zeit[1]=$tmp[$minute];
				$zeit[2]=$tmp[$sekunde];
			}
		break;

		default: // alle Werte angegeben
			$zeit[0]=$tmp[$stunde];
			$zeit[1]=$tmp[$minute];
			$zeit[2]=$tmp[$sekunde];
		break;
	}
	// Stunden umwandeln
	$uebertrag=floor(((int) $zeit[2])/60);
	$zeit[2]=(string) (((int) $zeit[2])%60);
	$zeit[1]=(string) ((int) $zeit[1] + $uebertrag);

	$uebertrag=floor(((int) $zeit[1])/60);
	$zeit[1]=(string) (((int) $zeit[1])%60);
	$zeit[0]=(string) ((int) $zeit[0] + $uebertrag);

	// Zeit in korrektes Format umwandeln und zurückgeben
	if($typ == 'zeit')
	{
		$pos=strpos(strtolower($entry), 'pm');
		if($pos !== false)
			$zeit[0]=(string) (12 + ((int) $zeit[0])%12);
		if((($zeit[0] == 24) && ($zeit[1] > 0) && ($zeit[2] > 0)) || ($zeit[0] > 24))
			$zeit[0]=(string) (((int) $zeit[0])%24);

		for($i=0; $i<3; $i++)
		{
			while(strlen($zeit[$i]) < 2)
				$zeit[$i]='0'.$zeit[$i];
		}

		return($zeit[0].$zeit[1].$zeit[2]);
	}
//fxDebug($zeit,__LINE__.': $zeit');

	// Zeitspanne in Sekunden umwandeln und zurückgeben
	if(substr($typ,0,10) == 'zeitspanne')
		return $zeit[0]*3600 + $zeit[1]*60 + $zeit[2];
}

/**
 * validate_userformat
 * Unterfunktion zu validate_entry: Versucht eine Benutzereingabe in ein vor-
 * definiertes Format zu konvertieren, dabei werden folgende Zeichen akzeptiert:
 * X	=> Beliebiges alphanumerisches Zeichen an dieser Stelle
 * Z	=> Beliebige Zahl an dieser Stelle, falls nicht dann 0
 * 9	=> Beliebige Zahl an dieser Stelle
 * => Escape-Character, d.h. nächstes Zeichen wird einfach übernommen
 * -----------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $entry:		String, der überprüft werden soll
 * $err:		Variable, die im Fehlerfall die Fehlermeldungsnummer enthält
 * $format:		Benutzerformat, auf das überprüft werden soll
 * -----------------------------------------------------------------------------
 * Rückgabe:
 * °°°°°°°°°
 * - false im Fehlerfall, dann enthält $err die Fehlermeldung, sonst
 * - Konvertierter Wert
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $entry  - Mandatory parameter:                 ???PARAMETER???
 * @param various &$err   - Mandatory parameter:                 ???PARAMETER???
 * @param string  $format - Optional parameter (default = '9'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validate_userformat($entry, &$err, $format='9')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$nformat='';
	$GLOBALS['err_param']='';

	// Formatgruppen ermitteln
	$s			= -1;
	$f			= -1;
	$fm_split	= array();
	$spos_add	= 0;
	for($spos=0; ($spos+$spos_add)<strlen($format); $spos++)
	{
		$cz=substr($format, $spos+$spos_add, 1);
		$fix=false;
		if($cz == '\\')
		{
			$spos_add++;
			$cz=substr($format, $spos+$spos_add, 1);
			$fix=true;
		}

		// Zahlen- oder Buchstabengruppe
		if($fix)
			$sf=0;
		else if($cz == 'X')
			$sf=1;
		else if($cz == '9')
			$sf=2;
		else if($cz == 'Z')
			$sf=3;
		else
			$sf=0;
		if($f != $sf)
		{
			$f=$sf;
			$s++;
			$fm_split[$s]['s']=$spos;
			$fm_split[$s]['l']=0;
			$fm_split[$s]['f']=$f;
			$fm_split[$s]['w']='';
		}
		$fm_split[$s]['l']++;
		$fm_split[$s]['w'] .= $cz;
		$nformat .= $cz;
	}
//fxDebug($fm_split, 'Formatgruppen von: '.$format.'<br />-> '.$nformat);

	// Prüfen, ob gleiche Länge
	if(strlen($nformat) != strlen($entry))
	{
		if(fxIsArray($fm_split) && (sizeof($fm_split) == 1) && ($fm_split[0]['f'] == 2) && (strlen($entry) < strlen($format)))
			$entry=substr(str_repeat('0', strlen($format)).$entry, -strlen($format));
		else
		{
			$err=912;	// <b>Benutzerformat-Fehler:</b> Zeichenlänge des Eingabetextes ($var1) ist ungleich der Länge vom Format "$var2" ($var3)!
			$GLOBALS['err_param']=array($entry, $nformat, strlen($nformat));
			return false;
		}
	}

	// Ausgabestring erstellen
	if(fxIsArray($fm_split))
	{
		$be		= '';
		$eas	= '';
		$fas	= '';
		foreach($fm_split as $bn => $fs)
		{
			$verr=false;
			for($z=0; $z<$fs['l']; $z++)
			{
				$ce=substr($entry, $fs['s']+$z, 1);
				if(!strlen($ce))
					$verr=true;
				else if(!$fs['f'])		// fix. Text
				{
					if($ce != substr($fs['w'], $z, 1))
						$verr=true;
				}
				else if($fs['f'] == 1)	// Buchstaben
				{
					if(is_numeric($ce))
						$verr=true;
				}
				else if($fs['f'] == 2)	// Zahl
				{
					if(!is_numeric($ce))
						$verr=true;
				}

				if($verr)
					break;
			}
			if($verr)
			{
				if(strlen($be))
					$be .= ', ';
				$be .= $bn;
				$eas .= '<font class="s1b" color="#880000">'.substr($entry, $fs['s'], $fs['l']).'</font>';
				$fas .= '<font class="s1b" color="#000088">'.$fs['w'].'</font>';
			}
			else
			{
				$eas .= substr($entry, $fs['s'], $fs['l']);
				$fas .= $fs['w'];
			}
		}

		if(strlen($be))
		{
			$err=899;	// <b>Benutzerformat-Fehler in Block Nr. $var1:</b> [$var2] ist nicht vom Format "$var3"!
			$GLOBALS['err_param']=array($be, $eas, $fas);
			return false;
		}
	}

	return $entry;
}

/*
 * Return the values of user settings format for specific settings

 * @param string $setting - Mandatory parameter:  Desired setting, e.g. "date", "time" or "decimal"
 *
 * @return Value of user setting
*/
function getSettingFormat($setting)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$setting=strtolower(trim($setting));
	$format='';

	if(strlen($setting))
	{
		// Alternatives
		$aa=array(
			'formatdatum'		=> array('datum','date'),
			'formatzeit'		=> array('zeit','time'),
			'deztrennzeichen'	=> array('dezimal','dezimalzeichen','decimal','decimalsign','period')
		);

		foreach($aa as $sname => $sa)
		{
			if($setting == $sname)
			{
				if(fxIsArray($GLOBALS['fxpglobals']['settings']) && isset($GLOBALS['fxpglobals']['settings'][$sname]) && strlen($GLOBALS['fxpglobals']['settings'][$sname]))
					$format=$GLOBALS['fxpglobals']['settings'][$sname];
				break;
			}
			else
			{
				foreach($sa as $aname)
				{
					if($setting == $aname)
					{
						$setting=$sname;
						if(fxIsArray($GLOBALS['fxpglobals']['settings']) && isset($GLOBALS['fxpglobals']['settings'][$sname]) && strlen($GLOBALS['fxpglobals']['settings'][$sname]))
							$format=$GLOBALS['fxpglobals']['settings'][$sname];
						break;
					}
				}
			}
		}

		if(!strlen($format) && isset($aa[$setting]))
		{
			$lang=fxGetLanguage(0,false);
			switch($setting)
			{
				case 'formatdatum':
					if($lang == 1)
						$format='dd.mm.yyyy';
					else
						$format='mm/dd/yyyy';
				break;

				case 'formatzeit':
					$format='HH:nn';
				break;

				case 'deztrennzeichen':
					if($lang == 1)
						$format=',';
					else
						$format='.';
				break;
			}
		}
	}

	return $format;
}

/**
 * get_entry
 * Funktion wandelt einen Datenbank-Eintrag in das gewünschte Anzeigeformat um.
 * -----------------------------------------------------------------------------
 * Zu übergebende Variablen:
 * °°°°°°°°°°°°°°°°°°°°°°°°°
 * $entry:		Datenbankstring, der ins Anzeigeformat umgewandelt werden soll
 * $feldarray:	<optional>	Feldtyp, der überprüft werden soll als string
 * (Standard: 'text'), oder Maskenfeldarray, dass die
 * nötigen Infos enthält wie feldtyp, nachkomma
 * $nachkomma:	<optional>	Anzahl der Nachkommastellen bei Feldtyp 'dezimal'
 * oder 'betrag' (Standard: '2')
 * -----------------------------------------------------------------------------
 * Rückgabe:
 * °°°°°°°°°
 * Konvertierter String im Anzeigeformat
 * -----------------------------------------------------------------------------
 * Beispiel:
 * °°°°°°°°°
 * $neu=get_entry('20010312000000', 'datum');
 * -> liefert als Ergebnis zurück: '12.03.2001'
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $entry     - Mandatory parameter:                    ???PARAMETER???
 * @param string  $feldarray - Optional parameter (default = 'text'):  ???PARAMETER???
 * @param string  $nks       - Optional parameter (default = ''):      ???PARAMETER???
 * @param string  $cls       - Optional parameter (default = ''):      ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_entry($entry, $feldarray='text', $nks='', $cls='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $debug;

	$feldname='';

	if(fxIsArray($feldarray))
	{
//fxDebug($feldarray, $entry);
		$typ=$GLOBALS['_ftypes'][$feldarray['feldtyp']];
		if(substr($feldarray['nummer'], 0, 1) == 'R')	// Report
			$feldname=strtolower($feldarray['feldname']);
	}
	else
		$typ=$feldarray;

	if(strlen($nks))
		$nachkomma=(int)$nks;
	else
		$nachkomma=(int)$GLOBALS['fxpglobals']['settings']['nkstellen'];

	// Kein Text angegeben, d.h. im Listenfeld kein Eintrag vorhanden
	if(fxIsArray($entry))
		return $entry;
	else if(!strlen($entry))
	{
		if(fxIsArray($feldarray) && $feldarray['feldname'])
			$setnull=(bool)$GLOBALS[$feldarray['feldname'].'_sn'];
		else
			$setnull=false;
		if($setnull && (($typ == 'integer') || ($typ == 'ganzzahl') || ($typ == 'longinteger') || ($typ == 'ganzzahllang') || ($typ == 'decimal') || ($typ == 'dezimal') || ($typ == 'amount') || ($typ == 'betrag') || ($typ == 'budget_int') || ($typ == 'budget_ext')))
			$entry=0;
		else if(!isset($GLOBALS['app_lang']) && ((($typ == 'timespan') || ($typ == 'zeitspanne') || ($typ == 'timespanm') || ($typ == 'zeitspannem') || ($typ == 'timespant') || ($typ == 'zeitspannet') || ($typ == 'timespani') || ($typ == 'zeitspannei')) && (!isset($GLOBALS['_ext_pdf']) || !strlen($GLOBALS['_ext_pdf']))))
		{
			if(!isset($GLOBALS['tshdc']))
				$GLOBALS['tshdc']=0;
			$GLOBALS['tshdc']++;
			$id='tshd_'.$GLOBALS['tshdc'];

			return '<span id='.$id.' sec=></span>';
		}
		else
			return '';
	}
	else
		$entry=convert_string($entry, 'fromdb');

	if($debug > 1)
		$rs='Typ: ('.$typ.') -> ';
	else
		$rs='';

	// Wenn Feld nicht umgewandelt werden soll, dann unverändert zurückgeben
	if(substr($entry, 0, 1) == '^') // CHANGE
	{
//echo('<hr>get_entry<i>('.$entry.', '.$feldname.', '.$nks.')</i><br />-&gt; <b style="color:red;">'.$rs.substr($entry, 1).'</b><br />');
		return $rs.substr($entry, 1);
	}

	// Einige Report-Felder speziell behandeln
	if($feldname)
	{
		if($feldname == 'vorgangsnummer')
			return no_null($entry);
		else if(substr($feldname, 0, 7) == 'aufwand')
			$entry=(double)$entry / 3600.0;
	}

	// Text anhand des Feldtyps auswerten
//echo('Typ: '.$typ.', $entry=['.$entry.'], $feldname=['.$feldname.']<br />');
	switch($typ)
	{
		case 'yes/no':
		case 'ja/nein':
			if((int)$entry)
				$rs .= '1';
			else
				$rs .= '0';
		break;

		case 'integer':
		case 'ganzzahl':
		case 'longinteger':
		case 'ganzzahllang':
			$entry=get_double($entry);
			$rs .= (string)(round($entry, 0));
		break;

		case 'decimal':
		case 'dezimal':
		case 'amount':
		case 'betrag':
		case 'budget_int':
		case 'budget_ext':
			$dez=getSettingFormat('deztrennzeichen');
			$entry=get_double($entry);
			if($dez == ',')
				$tsd_tz='.';
			else
				$tsd_tz=',';
			$rs .= (string)(number_format(round($entry, $nachkomma), $nachkomma, $dez, $tsd_tz));
		break;

		case 'integer_sp':
		case 'ganzzahl_tz':
			$dez=getSettingFormat('deztrennzeichen');
			$entry=get_double($entry);
			if($dez == ',')
				$tsd_tz='.';
			else
				$tsd_tz=',';
			$rs .= (string)(number_format(round($entry,0), 0, $dez, $tsd_tz));
		break;

		case 'date':
		case 'datum':
			$rs .= (string)(formatdatetime(getSettingFormat('formatdatum'), $entry));
		break;

		case 'time':
		case 'zeit':
			$rs .= (string)(formatdatetime(getSettingFormat('formatzeit'), substr('00000000000000'.$entry,-14)));
		break;

		case 'datetime':
		case 'datetimesec':
			$rs .= (string)(formatdatetime(getSettingFormat('formatdatum'), $entry)).', ';
			if($typ == 'datetimesec')
				$rs .= (string)(formatdatetime('HH:mm:ss', $entry));
			else
				$rs .= (string)(formatdatetime(getSettingFormat('formatzeit'), $entry));
		break;

		case 'timespan':
		case 'zeitspanne':
		case 'timespanm':
		case 'zeitspannem':
		case 'timespant':
		case 'zeitspannet':
		case 'timespani':
		case 'zeitspannei':
			$vorz='';
			if($entry < 0)
				$vorz='-';
			$entry=abs($entry);
			if(isset($GLOBALS['Zeiteinheit']))
			{
				$z_name='Zeiteinheit';
				if(($typ == 'zeitspannem') || ($typ == 'timespanm'))
					$z_pre=856;
				else
					$z_pre=484;
				$zanz=array(-3 => array('s', 6), 856 => array('m', 5), 484 => array('h', 4), 485 => array('mt', 3), 923 => array('mw', 2), 486 => array('mm', 1), 487 => array('mj', 0));
			}
			else
			{
				$z_name='zeitanzeige';
				if(($typ == 'zeitspannem') || ($typ == 'timespanm'))
					$z_pre=-2;
				else
					$z_pre=916;
				$zanz=array(-3 => array('s', 6), -2 => array('m', 5), 916 => array('h', 4), 917 => array('mt', 3), -1 => array('mw', 2), 918 => array('mm', 1), 919 => array('mj', 0));
			}

			$zs=array
			(
				array($GLOBALS['fxpglobals']['settings']['mjahr']*$GLOBALS['fxpglobals']['settings']['mtag']*3600, $GLOBALS['fxpglobals']['settings']['mmonat']*$GLOBALS['fxpglobals']['settings']['mtag']*3600, $GLOBALS['fxpglobals']['settings']['mwoche']*$GLOBALS['fxpglobals']['settings']['mtag']*3600, $GLOBALS['fxpglobals']['settings']['mtag']*3600, 3600, 60, 1),
				array(1, $GLOBALS['fxpglobals']['settings']['mjahr']/$GLOBALS['fxpglobals']['settings']['mmonat'], $GLOBALS['fxpglobals']['settings']['mmonat']/$GLOBALS['fxpglobals']['settings']['mwoche'], $GLOBALS['fxpglobals']['settings']['mwoche'], $GLOBALS['fxpglobals']['settings']['mtag'], 60, 60)
			);

			if(!isset($GLOBALS[$z_name]) || ($GLOBALS[$z_name] == ''))
				$z=$z_pre;
			else
			{
				$z=(int)$GLOBALS[$z_name];
				if(!isset($zanz[$z]))
					$z=$z_pre;
			}

			if($z == 0)
				$z=$z_pre;

			// Unwandeln von Sekunden -> Jahr, Monat, Woche, Tag - Stunden, Minuten, Sekunden
			$zz=(int)$entry;
			for($i=0; $i<7; $i++)
			{
				$tmp[$i]=floor($zz/$zs[0][$i]);
				$zz -= $tmp[$i]*$zs[0][$i];
			}
//echo("J0:{$tmp[0]}, M1:{$tmp[1]}, W2:{$tmp[2]}, T3:{$tmp[3]} -- h4:{$tmp[4]}, m5:{$tmp[5]}, s6:{$tmp[6]}<br />");

			if($zanz[$z][1] > 0) // Obere Felder dazuaddieren
			{
				for($i=0; $i<$zanz[$z][1]; $i++)
				{
					$tmp[$i+1] += $tmp[$i]*$zs[1][$i+1];
					$tmp[$i]=0;
				}
			}
//echo("&rarr; J0:{$tmp[0]}, M1:{$tmp[1]}, W2:{$tmp[2]}, T3:{$tmp[3]} -- h4:{$tmp[4]}, m5:{$tmp[5]}, s6:{$tmp[6]}<br />");

			// Y'M'T
			if($zanz[$z][1] < 4) // nur wenn mind. Manntage gewählt
			{
				$wert=(float) ((int)$entry/$zs[0][$zanz[$z][1]]);
				$rs .= get_entry($wert, 'dezimal').' ';
				if($z < 0)
					$rs .= $zanz[$z][0];
				else
					$rs .= meldung($z);
			}
			// H:M[:S]
			else
			{
				$dez=getSettingFormat('deztrennzeichen');

				$tmp[4]=round($tmp[4]);
				$tmp[5]=substr('00'.round($tmp[5]), -2);
				$tmp[6]=substr('00'.round($tmp[6]), -2);
				$tmp[7]=round(((((int)$tmp[5] * 100) / 60) + (((int)$tmp[6] * 100) / 3600)));
//echo("&rarr; h4:{$tmp[4]}, m5:{$tmp[5]}, s6:{$tmp[6]}, ms7:{$tmp[7]}<br />");

				$usedf=false;
				if(($typ == 'timespani') || ($typ == 'zeitspannei'))
					$usedf=true;
				else if(($typ == 'timespant') && ($typ != 'zeitspannet') && ($GLOBALS['fxpglobals']['settings']['zeitspanne_dez'] == 'Ja'))
					$usedf=true;

				if($usedf)
					$einst='HH'.$dez.'ii';
				else
					$einst=getSettingFormat('formatzeit');

				if(strpos($einst,'ii') !== false)
				{
					while($tmp[7] >= 100)
					{
						$tmp[4]++;
						$tmp[7] -= 100;
					}
				}
				$tmp[7]=substr('00'.$tmp[7], -2);

				// und entsprechend dem Benutzerzeitformat anzeigen
				if($dez == ',')
					$tsd_tz='.';
				else
					$tsd_tz=',';
				$ztrans['HH']=(string)(number_format(round($tmp[4],0), 0, $dez, $tsd_tz));
				$ztrans['nn']=$tmp[5];
				$ztrans['ss']=$tmp[6];
				$ztrans['ii']=$tmp[7];
//fxDebug($ztrans,'$sztrans');

				$aa=0;
				if(fxIsArray($feldarray))
				{
					$aa=(int)$feldarray['anzeigeart'];
//fxDebug($feldarray,'$feldarray: $entry='.$entry.', $aa='.$aa);
				}

				if(isset($GLOBALS['app_lang']) || (isset($GLOBALS['_ext_pdf']) && strlen($GLOBALS['_ext_pdf'])))
					$rs .= $vorz.strtr($einst, $ztrans);
				else
				{
					if(!isset($GLOBALS['tshdc']))
						$GLOBALS['tshdc']=0;
					$GLOBALS['tshdc']++;
					$id='tshd_'.$GLOBALS['tshdc'];

					if(strlen($cls))
						$cls=' class='.$cls;

					$rs .= '<span id='.$id.' sec='.$vorz.$entry.$cls.'>'.$vorz.strtr($einst, $ztrans).'</span>';
				}
			}
//echo('$entry='.$entry.' ($typ='.$typ.') -&gt; $rs='.fxHtmlEncode($rs).'<br />');
		break;

		default:
			$rs .= $entry;
	}

	return $rs;
}

/**
 * ???FUNCTION???
 *
 * @param various $str - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function time2sec($str)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$str_len=strlen($str);
	$vs='';
	$ts='';
	$tg=false;
	for($i=0; $i<$str_len; $i++)
	{
		$c=substr($str,$i,1);
		if($c == '<')
			$tg=true;
		else if($c == '>')
			$tg=false;
		else if(!$tg)
		{
			if(($c >= '0') && ($c <= '9'))
				$vs .= $c;
			else if($c == ':')
			{
				$vs_len=strlen($vs);
				if(!$vs_len)
					$vs='00';
				else if($vs_len == 1)
					$vs='0'.$vs;
				else
					$vs=substr($vs,0,2);
				$ts .= $vs;
				$vs='';
			}
		}
	}
	$vs_len=strlen($vs);
	if($vs_len)
	{
		if($vs_len == 1)
			$vs='0'.$vs;
		else
			$vs=substr($vs,0,2);
		$ts .= $vs;
	}
	$ts = substr($ts.'000000',0,6);

	$h=(int)cutLeadingZeros(substr($ts,0,2))*3600;
	$m=(int)cutLeadingZeros(substr($ts,2,2))*60;
	$s=(int)cutLeadingZeros(substr($ts,4,2));

	$t=min(86400,$h+$m+$s);
//echo('time2sec: $ts='.$ts.' -- $t='.$t);

	return $t;
}

/**
 * ???FUNCTION???
 *
 * @param various $sec - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $alz - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $ds  - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $sep - Optional parameter (default = ':'):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function sec2time($sec, $alz=true, $ds=false, $sep=':')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($alz)
		$sec=min(86400,$sec);

	$ls='';
	if($sec < 0)
	{
		$ls='-';
		$sec=abs($sec);
	}

	$h=floor($sec/3600);
	$m=floor(($sec - $h*3600)/60);
	$s=$sec - $h*3600 - $m*60;

	if(!$ds && ($s > 30))
	{
		$s=0;
		$m++;
		if($m > 59)
		{
			$m=0;
			$h++;
		}
	}

	$t=$ls;
	if($alz)
		$t .= addLeadingZeros($h,2);
	else
		$t = get_entry($h,'ganzzahl');
	$t .= $sep.addLeadingZeros($m,2);
	if($ds)
		$t .= $sep.addLeadingZeros($s,2);

//echo('sec2time: $sec='.$sec.' ($h='.$h.', $m='.$m.', $s='.$s.') -- $t='.$t);
	return $t;
}

/**
 * ???FUNCTION???
 *
 * @param various $val - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function cutLeadingZeros($val)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$val)
		return 0;
	while(strlen($val) && (substr($val,0,1) == '0'))
		$val=substr($val,1);
	if(!strlen($val))
		$val='0';

	return (int)$val;
}

/**
 * ???FUNCTION???
 *
 * @param various $val - Mandatory parameter: ???PARAMETER???
 * @param various $len - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function addLeadingZeros($val, $len)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($len && (strlen($val) < $len))
	{
		while(strlen($val) < $len)
			$val='0'.$val;
	}
	$val=substr($val,-$len);

	return $val;
}

/**
 * Validate select field(s)
 *
 * @param various &$va - Mandatory parameter:                   ???PARAMETER???
 * @param various $fa  - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $dbg - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validateSelectFields(&$va, $fa, $dbg=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$serr=false;
	if(is_array($va) && sizeof($va) && is_array($fa) && sizeof($fa))
	{
		foreach($fa as $fn => $fp)
		{
			$fx='';	// Extra

			$mand=false;
			if(substr($fp,0,1) == '*')
			{
				$fp=substr($fp,1);
				$mand=true;
			}

			$fxs=strpos($fn,'|');	// with Extra?
			if($fxs)
			{
				$fx=substr($fn,0,$fxs);
				$fn=substr($fn,$fxs+1);

				if(is_array($_POST) && sizeof($_POST) && isset($_POST[$fp]))
				{
					$cms=strpos($_POST[$fp],'_');
					if($cms)
					{
						${$fx}=(int)substr($_POST[$fp],0,$cms);
						${$fn}=(int)substr($_POST[$fp],$cms+1);
					}
					else
					{
						$fx='';
						${$fn}=(int)$_POST[$fp];
					}
				}
				else
				{
					${$fx}=0;
					${$fn}=0;
				}
			}
			else if(is_array($_POST) && sizeof($_POST) && isset($_POST[$fp]))
				${$fn}=(int)$_POST[$fp];
			else
				${$fn}=0;

			if($mand && (${$fn} == 0))
			{
				$serr=true;
				$GLOBALS['err']=true;
				error_msg(734,false,$fp);
			}

			$va[$fn]=${$fn};
			if(strlen($fx))
			{
				$va[$fx]=${$fx};
				$_POST[$fn]=$va[$fx].'_'.$va[$fn];
			}
			else
				$_POST[$fn]=$va[$fn];

			if($dbg)
			{
				if(strlen($fx))
					echo('<font class="grey">SELECT (prefix)</font> $'.$fx.' = <b>'.$va[$fx].'</b><br />');
				echo('<font class="grey">SELECT</font> $'.$fn.' = <b>'.$va[$fn].'</b><br />');
			}
		}
		if($dbg)
			echo('<hr />');
	}

	return $serr;
}

/**
 * Validate text field(s)
 *
 * @param various &$va - Mandatory parameter:                   Reference to the destination array where the validated values are stored
 * @param various $fa  - Mandatory parameter:                   Source array containing keynames and fieldnames, with special types starting with "@"=email, "*"=mandatory + "!"=password
 * @param boolean $dbg - Optional parameter (default = false):  Display debug information for each validation (true) or not (false)
 *
 * @return True on error, else false
 */
function validateTextFields(&$va, $fa, $dbg=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$xerr=false;
	if(is_array($va) && sizeof($va) && is_array($fa) && sizeof($fa))
	{
		foreach($fa as $fn => $fp)
		{
			// Email/Mandatory/Encoded
			$type='text';
			$mand='0';
			$enc=false;
			$f1=substr($fp,0,1);
			while(($f1 == '@') || ($f1 == '*') || ($f1 == '!'))
			{
				if($f1 == '@')
					$type='email';
				else if($f1 == '*')
					$mand='1';
				else
					$enc=true;
				$fp=substr($fp,1);
				$f1=substr($fp,0,1);
			}

			${$fn}='';
			if(fxIsArray($_POST) && isset($_POST[$fp]))
			{
				if($enc)
					${$fn}=fxDecodeValue($_POST[$fp]);
				else
					${$fn}=$_POST[$fp];
			}
			$_POST[$fp]=validate_entry(${$fn},$verr,$type,$mand);
			if($verr)
			{
				$xerr=true;
				$GLOBALS['err']=true;
				error_msg($verr,false,$fp);
			}
			else
				${$fn}=trim($_POST[$fp]);

			$va[$fn]=${$fn};

			if($dbg)
				echo('<font class="grey">TEXT</font> $'.$fn.' = <b>'.$va[$fn].'</b><br />');
		}
		if($dbg)
			echo('<hr />');
	}

	return $xerr;
}

/**
 * Validate date field(s)
 *
 * @param various &$va - Mandatory parameter:                   ???PARAMETER???
 * @param various $fa  - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $dbg - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validateDateFields(&$va, $fa, $dbg=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$derr=false;
	if(is_array($va) && sizeof($va) && is_array($fa) && sizeof($fa))
	{
		foreach($fa as $fn => $fp)
		{
			$mand='0';
			if(substr($fp,0,1) == '*')
			{
				$fp=substr($fp,1);
				$mand='1';
			}

			${$fn}='';
			if(fxIsArray($_POST) && isset($_POST[$fp]))
				${$fn}=$_POST[$fp];
			$_POST[$fp]=validate_entry(${$fn},$verr,'datum',$mand);
			if($verr)
			{
				$derr=true;
				$GLOBALS['err']=true;
				error_msg($verr,false,$fp);
			}
			else
				${$fn}=trim($_POST[$fp]);

			$va[$fn]=${$fn};

			if($dbg)
				echo('<font class="grey">DATE</font> $'.$fn.' = <b>'.$va[$fn].'</b><br />');
		}
		if($dbg)
			echo('<hr />');
	}

	return $derr;
}

/**
 * Validate checkbox field(s)
 *
 * @param various &$va - Mandatory parameter:                   ???PARAMETER???
 * @param various $fa  - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $dbg - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validateCheckboxFields(&$va, $fa, $dbg=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cerr=false;
	if(is_array($va) && sizeof($va) && is_array($fa) && sizeof($fa))
	{
		foreach($fa as $fn => $fp)
		{
			$mand='0';
			if(substr($fp,0,1) == '*')
			{
				$fp=substr($fp,1);
				$mand='1';
			}

			${$fn}=0;
			if(fxIsArray($_POST) && isset($_POST[$fp]) && $_POST[$fp])
				${$fn}=1;
			$_POST[$fp]=validate_entry(${$fn},$verr,'ja/nein',$mand);
			if($verr)
			{
				$cerr=true;
				$GLOBALS['err']=true;
				error_msg($verr,false,$fp);
			}

			$va[$fn]=${$fn};
			if($va[$fn])
				$_POST[$fp]=$va[$fn];
			else if(fxIsArray($_POST) && isset($_POST[$fp]))
				unset($_POST[$fp]);

			if($dbg)
				echo('<font class="grey">CHECKBOX</font> $'.$fn.' = <b>'.$va[$fn].'</b><br />');
		}
		if($dbg)
			echo('<hr />');
	}

	return $cerr;
}

/**
 * Validate number field(s)
 *
 * @param various &$va - Mandatory parameter:                   ???PARAMETER???
 * @param various $fa  - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $dbg - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validateNumberFields(&$va, $fa, $dbg=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$nerr=false;
	if(is_array($va) && sizeof($va) && is_array($fa) && sizeof($fa))
	{
		foreach($fa as $fn => $fp)
		{
			$type='dezimal';
			$fts=strpos($fp,'|');	// with Type?
			if($fts)
			{
				if($fts == 1)	// ...type at the beginning
				{
					$ft=strtoupper(substr($fp,0,1));
					$fp=substr($fp,$fts+1);
				}
				else			// ...type at the end
				{
					$ft=strtoupper(substr($fp,$fts+1,1));
					$fp=substr($fp,0,$fts);
				}
				if(($ft == 'A') || ($ft == 'B'))
					$type='betrag';
				else if(($ft == 'I') || ($ft == 'G'))
					$type='ganzzahl';
			}

			$mand='0';
			if(substr($fp,0,3) == '-**')
			{
				$fp=substr($fp,3);
				$mand='-2';
			}
			else if(substr($fp,0,2) == '-*')
			{
				$fp=substr($fp,2);
				$mand='-1';
			}
			else if(substr($fp,0,2) == '**')
			{
				$fp=substr($fp,2);
				$mand='2';
			}
			else if(substr($fp,0,1) == '*')
			{
				$fp=substr($fp,1);
				$mand='1';
			}

			$pk='';
			$fbs=strpos($fp,'[');
			if($fbs)
			{
				$pk=substr($fp,$fbs);
				$fp=substr($fp,0,$fbs);
			}

			$es  = '$'.$fn.'=\'\'; '.$GLOBALS['nl'];
			$es .= 'if(fxIsArray($_POST) && isset($_POST[\''.$fp.'\']'.$pk.')) '.$GLOBALS['nl'];
			$es .= '	$'.$fn.'=$_POST[\''.$fp.'\']'.$pk.'; '.$GLOBALS['nl'];
			$es .= '$_POST[\''.$fp.'\']'.$pk.'=validate_entry($'.$fn.',$verr,$type,$mand); '.$GLOBALS['nl'];
			$es .= 'if($verr) '.$GLOBALS['nl'];
			$es .= '{ '.$GLOBALS['nl'];
			$es .= '	$nerr=true; '.$GLOBALS['nl'];
			$es .= '	$GLOBALS[\'err\']=true; '.$GLOBALS['nl'];
			$es .= '	if(isset($GLOBALS[\'val_maskennummer\'])) '.$GLOBALS['nl'];
			$es .= '		$GLOBALS[\'val_fmpos\']=array(\'mnr\'=>$GLOBALS[\'val_maskennummer\'], \'feld\'=>\''.$fp.'\'); '.$GLOBALS['nl'];
			$es .= '	error_msg($verr,false,\''.$fp.'\'); '.$GLOBALS['nl'];
			$es .= '	if(isset($GLOBALS[\'val_maskennummer\'])) '.$GLOBALS['nl'];
			$es .= '		unset($GLOBALS[\'val_fmpos\']); '.$GLOBALS['nl'];
			$es .= '	$va[\''.$fn.'\']=$_POST[\''.$fp.'\']'.$pk.'; '.$GLOBALS['nl'];
			$es .= '} '.$GLOBALS['nl'];
			$es .= 'else '.$GLOBALS['nl'];
			$es .= '{ '.$GLOBALS['nl'];
			$es .= '	$'.$fn.'=trim($_POST[\''.$fp.'\']'.$pk.'); '.$GLOBALS['nl'];
			$es .= '	if($type == \'ganzzahl\') '.$GLOBALS['nl'];
			$es .= '		$'.$fn.'=(int)$'.$fn.'; '.$GLOBALS['nl'];
			$es .= '	else '.$GLOBALS['nl'];
			$es .= '		$'.$fn.'=get_double($'.$fn.'); '.$GLOBALS['nl'];
			$es .= '	$va[\''.$fn.'\']=$'.$fn.'; '.$GLOBALS['nl'];
			$es .= '	$_POST[\''.$fp.'\']'.$pk.'=$va[\''.$fn.'\']; '.$GLOBALS['nl'];
			$es .= '}';
//echo('<pre>'.fxHtmlEncode($es).'</pre>');

			eval($es);

			if($dbg)
				echo('<font class="grey">NUMBER ('.$type.') {$fp='.$fp.', $pk='.$pk.'}</font> $'.$fn.' = <b>'.$va[$fn].'</b><br />');
		}
		if($dbg)
			echo('<hr />');
	}

	return $nerr;
}

/**
 * Validate time field(s)
 *
 * @param various &$va - Mandatory parameter:                   ???PARAMETER???
 * @param various $fa  - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $dbg - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function validateTimeFields(&$va, $fa, $dbg=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$terr=false;
	if(is_array($va) && sizeof($va) && is_array($fa) && sizeof($fa))
	{
		foreach($fa as $fn => $fp)
		{
			$mand='0';
			if(substr($fp,0,1) == '*')
			{
				$fp=substr($fp,1);
				$mand='1';
			}

			${$fn}='';
			if(fxIsArray($_POST) && isset($_POST[$fp]))
				${$fn}=$_POST[$fp];
			$_POST[$fp]=validate_entry(${$fn},$verr,'zeit',$mand);
			if($verr)
			{
				$terr=true;
				$GLOBALS['err']=true;
				error_msg($verr,false,$fp);
			}
			else
				${$fn}=trim($_POST[$fp]);

			$va[$fn]=${$fn};

			if($dbg)
				echo('<font class="grey">TIME</font> $'.$fn.' = <b>'.$va[$fn].'</b><br />');
		}
		if($dbg)
			echo('<hr />');
	}

	return $terr;
}

/**
 * Handle uploaded person and material pictures
 *
 * @param string  $pm      - Mandatory parameter:                     Picture mode: 'PER'=Person + 'MAT'=Material
 * @param integer $ci      - Mandatory parameter:                     Client ID
 * @param integer $am      - Mandatory parameter:                     Action mode, e.g. FXP_CREATE or FXP_CHANGE
 * @param integer $id      - Mandatory parameter:                     Person or Material ID, depending on picture mode
 * @param array   $arr     - Optional parameter (default = array()):  Array of person or material picture name to be deleted
 * @param various $fn      - Optional parameter (default = ''):       Field name - if empty, the default names 'bild' and 'Datei' are asumed
 * @param boolean $dispmsg - Optional parameter (default = false):    Display messages?
 *
 * @return Success or error string in the format "{Code}|{Message}" with {Code}="OK" for success and "ERROR" for error
 */
function uploadPicture($pm, $ci, $am, $id, $arr=array(), $fn='', $dispmsg=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ufd=false;
	if(function_exists('uploadFilesDebug'))
		$ufd=true;

	$pm=substr(strtoupper($pm),0,3);
	$ci=(int)$ci;
	if(!$ci)
		$ci=$GLOBALS['fxpglobals']['client'];
	$am=(int)$am;
	$id=(int)$id;
	$ec='OK';
	$em=0;
	$ep='';
	$dpa=array();

	// Use which array
	$fn=trim($fn);
	$ffield='';
	$sfield='';
	$pfield='';
	$parr=false;
	if(fxIsArray($_FILES))
	{
		if(strlen($fn) && fxIsArray($_FILES[$fn]))
			$ffield=$fn;
		else if(fxIsArray($_FILES['bild']))
			$ffield='bild';
		else if(fxIsArray($_FILES['Datei']))
			$ffield='Datei';

		if(strlen($ffield))
		{
			$sfield='_FILES';
			$parr=$_FILES[$ffield];
		}
	}
	if(!strlen($ffield) && fxIsArray($_POST))
	{
		if(strlen($fn) && fxIsArray($_POST[$fn]))
			$pfield=$fn;
		else if(fxIsArray($_POST['bild']))
			$pfield='bild';
		else if(fxIsArray($_POST['Datei']))
			$pfield='Datei';

		if(strlen($pfield))
		{
			$sfield='_POST';
			$parr=$_POST[$pfield];
		}
	}
if($ufd) uploadFilesDebug("\$ffield=".$ffield.", \$pfield=".$pfield.", \$parr:\r\n".print_r($parr,true));

	// Create client person (= PER) or material (= MAT) folder
	$cdir=$GLOBALS['datpath'].$ci;
	if(!is_dir($cdir))
		mkdir($cdir, 0755);
	$pdir=$cdir;
	if(is_dir($pdir))
	{
		$cdir .= '/';
		$pdir .= '/'.$pm;
		if(!is_dir($pdir))
			mkdir($pdir, 0755);
	}
	if(is_dir($pdir))
		$pdir .= '/';
//fxDebug($cdir,'$cdir', 0); fxDebug($pdir,'$pdir', 0);
if($ufd) uploadFilesDebug('$cdir='.$cdir."\r\n\$pdir=".$pdir);

	// Create or edit picture
if($ufd) uploadFilesDebug('$am='.$am);
	if(($am == FXP_CREATE) || ($am == FXP_CHANGE))
	{
//fxDebug($_POST[$pfield], $pfield, 0);
		if(fxIsArray($parr) && $parr['size'])
		{
			// Copy picture(s) to client person or material folder
			if(is_dir($pdir) && !$parr['error'] && strlen($parr['new_name']) && file_exists($parr['new_name']))
			{
				// Picture names
				$pf_src=$pdir.$id.'_src.png';
				$pf_256=$pdir.$id.'_256.png';
				$pf_128=$pdir.$id.'_128.png';
				$pf_100=$pdir.$id.'_100.png';
				$pf_ico=$pdir.$id.'_ico.png';

				$im_upload=false;
				$pic_width=0;
				$pic_height=0;

				// Check source picture format and size
				// - 0: Width
				// - 1: Height
				// - 2: Type (IMAGETYPE_...): 1=GIF, 2=JPG, 3=PNG, 4=SWF, 5=PSD, 6=BMP, 7=TIFF(intel byte order), 8=TIFF(motorola byte order), 9=JPC, 10=JP2, 11=JPX, 12=JB2, 13=SWC, 14=IFF, 15=WBMP, 16=XBM
				$ims=GetImageSize($parr['new_name']);

				// Is it already a png picture?
				// ...Yes
				if($ims[2] == 3)
				{
					$pic_width=(int)$ims[0];
					$pic_height=(int)$ims[1];
				}
				// ...No, try to create one
				else if(function_exists('imagepng'))
				{
					// Create image
					if(($ims[2] == 1) && function_exists('imagecreatefromgif'))			// GIF
						$im_upload=@imagecreatefromgif($parr['new_name']);
					else if(($ims[2] == 2) && function_exists('imagecreatefromjpeg'))	// JPEG
						$im_upload=@imagecreatefromjpeg($parr['new_name']);
					else if(($ims[2] == 6) && function_exists('imagecreatefromwbmp'))	// BMP
						$im_upload=@imagecreatefromwbmp($parr['new_name']);
					if($im_upload)
					{
						$pic_width=(int)imagesx($im_upload);
						$pic_height=(int)imagesy($im_upload);
					}
				}

				if(!$pic_width || !$pic_height)
				{
					// 3189: The uploaded picture $var1 is not in PNG format and also couldn't be converted to it!
					$ec='ERROR';
					$em=3189;
					$ep='&quot;<b class=red>'.$parr['name'].'</b>&quot;';
				}
				else
				{
					if($im_upload)
						$success=imagepng($im_upload, $pf_src);
					else
						$success=copy($parr['new_name'], $pf_src);

					// Create icon and list picture
					if($success && function_exists('imagepng') && function_exists('imagecreatefrompng') && function_exists('imagecopyresampled'))
					{
						$ca=array(20=>$pf_ico, 100=>$pf_100, 128=>$pf_128, 256=>$pf_256);
						foreach($ca as $max_wh => $pf)
						{
							if($im_upload)
							{
								imagedestroy($im_upload);
								$im_upload=false;
							}
							$im_upload=@imagecreatefrompng($pf_src);
							if($im_upload)
							{
								// Get new dimensions
								if($pic_width < $pic_height)
								{
									$new_height=$max_wh;
									$new_width=min($new_height, ceil($pic_width*$new_height/$pic_height));
								}
								else
								{
									$new_width=$max_wh;
									$new_height=min($new_width, ceil($new_width*$pic_height/$pic_width));
								}
//echo('<b>'.$pf.'</b> ('.$max_wh.') -- $pic_width='.$pic_width.', $pic_height='.$pic_height.' -- $new_width='.$new_width.', $new_height='.$new_height.'<br />');

								if($max_wh == 20)	// Special treatment for icons
									$im_new=imagecreatetruecolor(20,20);
								else
									$im_new=imagecreatetruecolor($new_width,$new_height);
								if($im_new)
								{
									if(function_exists('imagealphablending'))
										imagealphablending($im_new,false);
									if(function_exists('imagesavealpha'))
										imagesavealpha($im_new,true);
									if($max_wh == 20)	// Special treatment for icons
									{
										if(function_exists('imagefilledrectangle') && function_exists('imagecolorallocatealpha'))
											imagefilledrectangle($im_new, 0,0, 19,19, imagecolorallocatealpha($im_new, 0,0,0, 127));
										$new_success=imagecopyresampled($im_new, $im_upload, max(0,floor((20-$new_width)/2)),max(0,floor((20-$new_height)/2)), 0,0, $new_width,$new_height, $pic_width,$pic_height);
									}
									else
										$new_success=imagecopyresampled($im_new, $im_upload, 0,0, 0,0, $new_width,$new_height, $pic_width,$pic_height);
									if($new_success)
										imagepng($im_new,$pf);
									imagedestroy($im_new);
									$im_new=false;
								}
							}
						}
					}

					if(!$success)
					{
						// 3190: The uploaded picture $var1 couldn't be added!
						$ec='ERROR';
						$em=3190;
						$ep='&quot;<b class=red>'.$parr['name'].'</b>&quot;';
					}
					else
					{
						// 3192: The picture $var1 was successfully uploaded and added.
						if($am == FXP_CREATE)
							$em=3192;
						// 3260: The picture $var1 was successfully uploaded and changed.
						else
							$em=3260;
						$ep='&quot;<b class=blue>'.$parr['name'].'</b>&quot;';

						// Create old picture delete list?
						if(fxIsArray($arr))
						{
							foreach($arr as $a)
							{
								$dpa[$cdir.'pic_'.$a.'_l.gif']=true;
								$dpa[$cdir.'pic_'.$a.'_l.jpg']=true;
								$dpa[$cdir.'pic_'.$a.'_l.png']=true;
								$dpa[$cdir.'pic_'.$a.'_s.gif']=true;
								$dpa[$cdir.'pic_'.$a.'_s.jpg']=true;
								$dpa[$cdir.'pic_'.$a.'_s.png']=true;
							}
						}
 					}
				}

				if($im_upload)
				{
					imagedestroy($im_upload);
					$im_upload=false;
				}
			}
			else
			{
				// 3191: The picture $var1 couldn't be uploaded!
				$ec='ERROR';
				$em=3191;
				$ep='"<b class=red>'.$parr['name'].'</b>"';
			}
		}

		// Rename old pictures?
		else if(fxIsArray($arr) && (sizeof($arr) == 2) && ($arr[0] !== $arr[1]))
		{
			$sla=array('s', 'l');
			foreach($sla as $sl)
			{
				$opi=$cdir.'pic_'.$arr[1].'_'.$sl.'.gif';
				if(file_exists($opi))
				{
					$npi=$cdir.'pic_'.$arr[0].'_'.$sl.'.gif';
					rename($opi, $npi);
				}
			}
		}
	}

	// Create delete picture list?
	else if($am == FXP_DELETE)
	{
		// ...Old pictures
		if(fxIsArray($arr))
		{
			foreach($arr as $a)
			{
				$dpa[$cdir.'pic_'.$a.'_l.gif']=true;
				$dpa[$cdir.'pic_'.$a.'_l.jpg']=true;
				$dpa[$cdir.'pic_'.$a.'_l.png']=true;
				$dpa[$cdir.'pic_'.$a.'_s.gif']=true;
				$dpa[$cdir.'pic_'.$a.'_s.jpg']=true;
				$dpa[$cdir.'pic_'.$a.'_s.png']=true;
			}
		}

		// ...New pictures
		$dpa[$pdir.$id.'_src.png']=true;
		$dpa[$pdir.$id.'_256.png']=true;
		$dpa[$pdir.$id.'_128.png']=true;
		$dpa[$pdir.$id.'_100.png']=true;
		$dpa[$pdir.$id.'_ico.png']=true;
	}

	// Delete pictures
	$dps=0;
	if(sizeof($dpa))
	{
		foreach($dpa as $dpf => $dummy)
		{
			if(file_exists($dpf))
			{
				@chmod($dpf, 0755);
				@unlink($dpf);
				$dps++;
			}
		}
	}

	if(($am == FXP_DELETE) && $dps)
	{
		// 3259: The picture was successfully deleted.
		$em=3259;
	}

	if(strlen($ep) && strlen($ffield))
		$ep='<span></span>';

	if($dispmsg && $em)
	{
		if(strlen($ep))
			error_msg($em,true,$ep);
		else
			error_msg($em,false);
	}

	$msg='';
	if($em)
	{
		if(strlen($ep))
			$msg=meldung($em,true,$ep);
		else
			$msg=meldung($em,false);
	}
	return $ec.'|'.$msg;
}

/**
 * ???FUNCTION???
 *
 * @param various $mid - Mandatory parameter:                   ???PARAMETER???
 * @param integer $mxh - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $cid - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $fit - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMaterialIcons($mid, $mxh=0, $cid=0, $fit=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=(int)$mid;
	$mxh=(int)$mxh;

	if(!isset($GLOBALS['_maticar']) || !is_array($GLOBALS['_maticar']))
		$GLOBALS['_maticar']=array();

	if(isset($GLOBALS['_maticar'][$mid]) && ($GLOBALS['_maticar'][$mid]['mxh'] == $mxh))
		return $GLOBALS['_maticar'][$mid];

	$cid=(int)$cid;
	if(!$cid)
		$cid=$GLOBALS['fxpglobals']['client'];

	if(!isset($GLOBALS['_maticar'][$mid]))
	{
		$GLOBALS['_maticar'][$mid]=array(
			'mid'=>$mid, 'mxh'=>$mxh,
			'def'=>true, 'mtime'=>'?',
			'src'=>'', '256'=>'', '128'=>'', '100'=>'', 'ico'=>'',
			'size_src'=>'', 'size_256'=>'', 'size_128'=>'', 'size_100'=>'', 'size_ico'=>'',
			'dsp'=>'', 'size_dsp'=>''
		);

		// New format (PNG)...
		$npd=$GLOBALS['datpath'].$cid.'/MAT/';
		$npico=$npd.$mid.'_ico.png';
		$np100=$npd.$mid.'_100.png';
		$np128=$npd.$mid.'_128.png';
		$np256=$npd.$mid.'_256.png';
		$npsrc=$npd.$mid.'_src.png';
//echo('$npico='.$npico.', $np100='.$np100.', $np128='.$np128.', $np256='.$np256.', $npsrc='.$npsrc.'<hr />');

		// ...exists
		if(file_exists($npico) || file_exists($np100) || file_exists($np128) || file_exists($np256) || file_exists($npsrc))
		{
			$GLOBALS['_maticar'][$mid]['def']=false;

			if(file_exists($npico))
				$GLOBALS['_maticar'][$mid]['ico']=$npico;
			if(file_exists($np100))
				$GLOBALS['_maticar'][$mid]['100']=$np100;
			if(file_exists($np128))
				$GLOBALS['_maticar'][$mid]['128']=$np128;
			if(file_exists($np256))
				$GLOBALS['_maticar'][$mid]['256']=$np256;
			if(file_exists($npsrc))
				$GLOBALS['_maticar'][$mid]['src']=$npsrc;
		}
		else
		{
			$GLOBALS['_maticar'][$mid]['100']=$GLOBALS['gfxpath'].'mat_b_100x100.png';
			$GLOBALS['_maticar'][$mid]['ico']=$GLOBALS['gfxpath'].'mat_b_20x20.png';
		}

		if(!strlen($GLOBALS['_maticar'][$mid]['ico']))
		{
			if(strlen($GLOBALS['_maticar'][$mid]['100']))
				$GLOBALS['_maticar'][$mid]['ico']=$GLOBALS['_maticar'][$mid]['100'];
			else if(strlen($GLOBALS['_maticar'][$mid]['128']))
				$GLOBALS['_maticar'][$mid]['ico']=$GLOBALS['_maticar'][$mid]['128'];
			else if(strlen($GLOBALS['_maticar'][$mid]['256']))
				$GLOBALS['_maticar'][$mid]['ico']=$GLOBALS['_maticar'][$mid]['256'];
			else if(strlen($GLOBALS['_maticar'][$mid]['src']))
				$GLOBALS['_maticar'][$mid]['ico']=$GLOBALS['_maticar'][$mid]['src'];
		}
	}

	// Sizes
	$sa=array('ico', '100', '128', '256', 'src');
	$ma=array();
	foreach($sa as $st)
	{
		if(strlen($GLOBALS['_maticar'][$mid][$st]))
		{
			$ims=GetImageSize($GLOBALS['_maticar'][$mid][$st]);
//fxDebug($ims,$GLOBALS['_maticar'][$mid][$st].' -- $mxh='.$mxh, 0);
			if(fxIsArray($ims))
			{
				$w=$ims[0];
				$h=$ims[1];
				$sz=' width='.$w.' height='.$h;

				if($mxh && ($h != $mxh))
				{
					$w=round(($mxh*$w)/$h);
					$h=$mxh;
				}
				if(($st != 'ico') && $fit)
					$sz=' width='.$w.' height='.$h;

				$ma[$st]=array('dh'=>abs($mxh-$ims[1]), 'df'=>$ims[1]/$h, 'ow'=>$ims[0], 'oh'=>$ims[1], 'nw'=>$w, 'nh'=>$h, 'sz'=>$sz);
			}
			else
				$sz=' width=20 height=20';

			$GLOBALS['_maticar'][$mid]['size_'.$st]=$sz;
		}
	}

	if(fxIsArray($ma))
	{
		$dh=1048576;
		foreach($ma as $st => $sa)
		{
			if(!strlen($ds) || ($sa['dh'] < $dh))
			{
				$dh=$sa['dh'];
				$ds=$st;
				$GLOBALS['_maticar'][$mid]['dsp']=$GLOBALS['_maticar'][$mid][$ds];

				if($fit)
					$GLOBALS['_maticar'][$mid]['size_dsp']=' width='.$sa['nw'].' height='.$sa['nh'];
				else
					$GLOBALS['_maticar'][$mid]['size_dsp']=$GLOBALS['_maticar'][$mid]['size_'.$ds];
			}
		}
	}

	// Last modify time
	if(strlen($GLOBALS['_maticar'][$mid]['ico']))
		$GLOBALS['_maticar'][$mid]['mtime'] .= filemtime($GLOBALS['_maticar'][$mid]['ico']);

//fxDebug($GLOBALS['_maticar'][$mid],'$GLOBALS['_maticar'][$mid]', 0);
	return $GLOBALS['_maticar'][$mid];
}

/**
 * ???FUNCTION???
 *
 * @param various $str      - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $sepstyle - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function extractTooltip($str,$sepstyle=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$tt1='';
	$tt2='';
	if(strlen($str))
	{
		$tts=strpos($str,'<tt>');
		if($tts !== false)
		{
			$tte=strpos($str,'</tt>',$tts+4);
			if($tte !== false)
			{
				$tt1=trim(substr($str,$tts+4,$tte-$tts-4));
				if($sepstyle)
					$tt2=' style="cursor:help;" tooltip="'.$tt1.'"';
				else
					$tt2='cursor:help;" tooltip="'.$tt1;
				if($tts > 0)
					$str=substr($str,0,$tts).substr($str,$tte+5);
				else
					$str=substr($str,$tte+5);
			}
		}
	}
	return array('text'=>$str, 'tooltip1'=>$tt1, 'tooltip2'=>$tt2);
}

/**
 * ???FUNCTION???
 *
 * @param various $setting - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getSettingValue($setting)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$value=false;
	if(fxIsArray($GLOBALS['fxpglobals']['settings']) && isset($GLOBALS['fxpglobals']['settings'][$setting]))
		$value=$GLOBALS['fxpglobals']['settings'][$setting];
	else
	{
		$defaults=array(
			'max_pic_sel' => 250
		);
		if(isset($defaults[$setting]))
			$value=$defaults[$setting];
	}

	// Return Yes/No values as 1/0
	if(is_string($value))
	{
		$lvalue=strtolower(trim($value));
		if($lvalue == 'ja')
			return 1;
		if($lvalue == 'nein')
			return 0;
	}

	// Adjust some values
	if($setting == 'max_pic_sel')
		$value=max(0,$value);

	return $value;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $return_error - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getLanguages($return_error=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Current language
	$lar=array($GLOBALS['fxpglobals']['lang']=>'Default');

	// All available languages
	$sql="SELECT wertetabid, tabwert FROM wertetab WHERE mandanten_id=0 AND id_feld=53 AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$lta=db_values($sql, '*');
//fxDebug($lta,$sql);

	if(fxIsArray($lta))
	{
		foreach($lta as $l)
			$lar[(int)$l['wertetabid']]=$l['tabwert'];
	}
	else if($return_error)
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$msg='Es sind (noch) keine Sprachen in der Datenbank definiert!';
		else
			$msg='There are no languages defined in the database (yet)!';
		return 'E'.$msg;
	}

//fxDebug($lar,'$lar');
	return $lar;
}

/**
 * ???FUNCTION???
 *
 * @param various $id       - Mandatory parameter:                 ???PARAMETER???
 * @param string  $value    - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $position - Optional parameter (default = ''):   ???PARAMETER???
 * @param integer $field    - Optional parameter (default = 255):  ???PARAMETER???
 * @param integer $lang     - Optional parameter (default = 0):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function clientLookupValue($id, $value='', $position='', $field=255, $lang=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$id=(int)$id;
	$value=trim($value);
	$position=trim($position);
	$field=(int)$field;
	$lang=(int)$lang;
	if(!$lang)
		$lang=$GLOBALS['fxpglobals']['lang'];

	// All available languages
	$lar=getLanguages(true);
	if(!fxIsArray($lar))
		return $lar;

	// INSERT
	if(!$id)
	{
		$wid=id_max('wertetab', 'wertetabid', '', $GLOBALS['fxpglobals']['client']);

		foreach($lar as $l => $lt)
		{
			// Language + Value in language
			$v=$value;
			if($l != $lang)
				$v='§'.$v;
			if(strlen($position))
				$p=(int)$position;
			else
				$p=0;

			$sql  = "INSERT INTO wertetab";
			$sql .= " (mandanten_id, wertetabid, id_sprache, id_feld, position, tabwert, satzvers, archiv) ";
			$sql .= "VALUES";
			$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$wid.", ".$l.", ".$field.", ".$p.", ".db_convert_string($v).", 0, 0)";
			$err=db_x($sql);
//fxDebug($err,$sql);

			if($err)
			{
				$ev=$v;
				$ep=strpos($ev,'^');
				if($ep)
					$ev=substr($ev,0,$ep);

				if($GLOBALS['fxpglobals']['lang'] == 1)
					$msg='Fehler beim Anlegen des Nachschlagewertes &quot;<b>'.$ev.'</b>&quot; in der Sprache <i>['.$lt.']</i>!';
				else
					$msg='Error while inserting the lookup value &quot;<b>'.$ev.'</b>&quot; in the language <i>['.$lt.']</i>!';
				return 'E'.$msg;
			}
		}

		return $wid;
	}
	// UPDATE/DELETE
	else
	{
		$m0='';
		if(strlen($value))
			$m0='0,';
		$sql="SELECT mandanten_id, id_sprache, id_feld, position, tabwert, satzvers, archiv FROM wertetab WHERE wertetabid=".$id." AND mandanten_id IN (".$m0.$GLOBALS['fxpglobals']['client'].") ORDER BY mandanten_id DESC";
		$war=db_values($sql,'*');
//fxDebug($war,$sql);
		if(!fxIsArray($war))
		{
			// Nothing to delete -> No error
			if(!strlen($value))
				return false;

			// Nothing to change -> Error
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$msg='Es sind (noch) keine Nachschlagewerte in der Datenbank definiert, welche verändert werden können!';
			else
				$msg='There are no lookup values defined in the database (yet) that can be changed!';
			return 'E'.$msg;
		}

		// UPDATE
		if(strlen($value))
		{
			foreach($lar as $l => $lt)
			{
				// Check if entry exists
				$e=-1;
				foreach($war as $w => $ar)
				{
					if(($ar['mandanten_id'] == $GLOBALS['fxpglobals']['client']) && ($ar['id_sprache'] == $l))
					{
						$e=$w;
						break;
					}
				}

				// Value in language
				$v=$value;
				if(($l != $lang) && (($e < 0) || (mb_substr($war[$e]['tabwert'],0,1) == '§')))
					$v='§'.$v;

				// Position
				if(strlen($position))
					$p=(int)$position;
				else
					$p=(int)$war[0]['position'];

				// INSERT
				if($e < 0)
				{
					$sql  = "INSERT INTO wertetab";
					$sql .= " (mandanten_id, wertetabid, id_sprache, id_feld, position, tabwert, satzvers, archiv) ";
					$sql .= "VALUES";
					$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$id.", ".$l.", ".(int)$war[0]['id_feld'].", ".$p.", ".db_convert_string($v).", ".(int)$war[0]['satzvers'].", ".(int)$war[0]['archiv'].")";
				}
				// UPDATE
				else
				{
					$sql  = "UPDATE wertetab SET position=".$p.", tabwert=".db_convert_string($v)." WHERE wertetabid=".$id." AND id_sprache=".$l." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				}
				$err=db_x($sql);
//fxDebug($err,$sql);

				if($err)
				{
					$ev=$v;
					$ep=strpos($ev,'^');
					if($ep)
						$ev=substr($ev,0,$ep);

					if($GLOBALS['fxpglobals']['lang'] == 1)
						$msg='Fehler beim Ändern des Nachschlagewertes &quot;<b>'.$ev.'</b>&quot; in der Sprache <i>['.$lt.']</i>!';
					else
						$msg='Error while changing the lookup value &quot;<b>'.$ev.'</b>&quot; in the language <i>['.$lt.']</i>!';
					return 'E'.$msg;
				}
			}

			return false;
		}
		// DELETE
		else
		{
			// Check if entry exists
			$value='<i>['.$id.']</i>';
			foreach($war as $w => $ar)
			{
				if($ar['id_sprache'] == $lang)
				{
					$ev=$ar['tabwert'];
					$ep=strpos($ev,'^');
					if($ep)
						$ev=substr($ev,0,$ep);

					$value='&quot;<b>'.$ev.'</b>&quot;';
					break;
				}
			}

			// Check if value is still in use
		 	$field=(int)$war[0]['id_feld'];
			$emsg=checkClientLookupValueUsage($id, $field);
			if(strlen($emsg))
			{
				if($GLOBALS['fxpglobals']['lang'] == 1)
					$msg='Fehler beim Löschen des Nachschlagewertes '.$value.'!';
				else
					$msg='Error while deleting the lookup value '.$value.'!';
				$msg .= '<br /><br />'.$emsg;
				return 'E'.$msg;
			}

			$sql="DELETE FROM wertetab WHERE wertetabid=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$err=db_x($sql);
//fxDebug($err,$sql);

			if($err)
			{
				if($GLOBALS['fxpglobals']['lang'] == 1)
					$msg='Fehler beim Löschen des Nachschlagewertes '.$value.'!';
				else
					$msg='Error while deleting the lookup value '.$value.'!';
				return 'E'.$msg;
			}

			return false;
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $id    - Mandatory parameter:                 ???PARAMETER???
 * @param integer $field - Optional parameter (default = 0):    ???PARAMETER???
 * @param string  $mode  - Optional parameter (default = 'm'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function checkClientLookupValueUsage($id, $field=0, $mode='m')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$id=(int)$id;
	$field=(int)$field;
	$mode=strtolower(substr(trim($mode),0,1));

	$used=false;
	$tables=array();
	$message='';

	// Get field?
	if(!$field)
	{
		$sql="SELECT id_feld FROM wertetab WHERE wertetabid=".$id." AND id_sprache=1 AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		$field=(int)db_value($sql);
	}

	if($id && $field)
	{
		// Check, if there exists a client 0 value
		$sql="SELECT wertetabid FROM wertetab WHERE wertetabid=".$id." AND id_sprache=1 AND mandanten_id=0";
		$v0=(int)db_value($sql);
//fxDebug($v0,$sql);

	 	// Get alias fields and usages except for default message field 255
	 	if(($field != 255) && !$v0)
	 	{
			// Alias fields
			$afields='';
			$sql="SELECT id FROM felder WHERE alias_zu=".$field." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
			$aa=db_values($sql, '*');
			if(fxIsArray($aa))
			{
				foreach($aa as $a)
					$afields .= ','.(int)$a['id'];
			}

			// All tables with this field (except history tables)
			$sql  = "SELECT tf.idintabellen, t.tabname, f.feldname";
			$sql .= " FROM tab_felder tf, tabellen t, felder f";
			$sql .= " WHERE tf.idintabellen=t.id_tabellen AND (t.id_tabellen>0 OR t.historie=0) AND tf.idinfelder=f.id AND tf.idinfelder IN (".$field.$afields.") AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
			$ta=db_values($sql, '*');
//fxDebug($ta,$sql);

			// Check, if field is used
			if(fxIsArray($ta))
			{
				foreach($ta as $t)
				{
					$tname=strtolower($t['tabname']);
					$sql="SELECT COUNT(".$t['feldname'].") AS cnt FROM ".$tname." WHERE ".$t['feldname']."=".$id;
					if($GLOBALS['fxpglobals']['dbparam']['ctables'][$tname])
						$sql .= " AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$cnt=(int)db_value($sql);
//fxDebug($cnt,'$cnt: $sql='.$sql);
					if($cnt)
					{
						$used=true;
						$tables[$t['tabname']]=$cnt;
					}
				}
			}
//fxDebug($tables,'$tables');

			if($used && sizeof($tables))
			{
				$message=meldung(1119,false,'',':');	// The value $var1cannot be deleted, because it is still used in the following tables$var2
				$message .= '<br /><br /><ul style=padding-left:16px;>';
				foreach($tables as $tname => $cnt)
					$message .= '<li>'.ucfirst($tname).' <i class=grey>('.$cnt.'x)</i></li>';
				$message .= '</ul>';
			}
		}
	}

	if($mode == 'u')
		return $used;
	if($mode == 't')
		return $tables;
	return $message;
}

/**
 * Set the person role (0=employee)
 */
function setPRole()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['fxpglobals']['persdat']['role']=0;
	if(isset($GLOBALS['fxpglobals']['persdat']['roles']))
		unset($GLOBALS['fxpglobals']['persdat']['roles']);
	$GLOBALS['fxpglobals']['persdat']['roles'][0]=true;

	$sql  = "SELECT mafaehigkeit_id FROM ma_art_faehigkeiten WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND mafaehigkeit_id<100";
	$sql .= " ORDER BY mafaehigkeit_id DESC";
	$tmp=db_values($sql, '*');
	// Sysadmin oder Admin haben autom. die Mitarbeiterrolle Geschäftsleitung
	if(!$GLOBALS['fxpglobals']['person'] || ($GLOBALS['fxpglobals']['user'] == 1))
		$tmp[]['mafaehigkeit_id']=2;
	if(fxIsArray($tmp))
	{
		foreach($tmp as $t)
		{
			if($GLOBALS['fxpglobals']['persdat']['role'] < (int)$t['mafaehigkeit_id'])
				$GLOBALS['fxpglobals']['persdat']['role']=(int)$t['mafaehigkeit_id'];
			$GLOBALS['fxpglobals']['persdat']['roles'][(int)$t['mafaehigkeit_id']]=true;
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $pid   - Mandatory parameter:                   ???PARAMETER???
 * @param integer $mxh   - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $sex   - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $cid   - Optional parameter (default = 0):      ???PARAMETER???
 * @param integer $ptype - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $fit   - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getPersonIcons($pid, $mxh=0, $sex=0, $cid=0, $ptype=0, $fit=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$name=$pid;
	$pid=(int)$pid;
	$mxh=(int)$mxh;
	$sex=(int)$sex;
	$cid=(int)$cid;
	$ptype=(int)$ptype;
	if(!$cid)
		$cid=$GLOBALS['fxpglobals']['client'];

	if(!isset($GLOBALS['_persicar']) || !is_array($GLOBALS['_persicar']))
		$GLOBALS['_persicar']=array();

	$id=$cid.'_'.$ptype.'_';
	if((string)$pid === (string)$name)
		$id .= $pid;
	else
		$id .= 0;

	if(!isset($GLOBALS['_persicar'][$id]))
	{
		$fname='';
		$sname='';
		$parent=0;
		$inact=false;
		$vfrom='';
		$vto='';

		$tmp=array();
		$sql="SELECT personen_id, pname, vorname, geschlecht, personentyp, elter, inaktiv, gueltig_von, gueltig_bis FROM personen WHERE ";
		if((string)$pid == (string)$name)
		{
			$sql .= "personen_id=".$pid." AND mandanten_id=".$cid;
			$tmp=db_values($sql);
		}
		else if(strlen($name))
		{
			$dbtype=$GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['type'];
			if($dbtype == 'sqlsrv')
				$sql .= db_like('(pname + vorname)',$name);
			else
				$sql .= db_like('(pname || vorname)',$name);
			$sql .= " AND mandanten_id=".$cid;
			$tmp=db_values($sql);
		}
		if(fxIsArray($tmp))
		{
//fxDebug($tmp,$sql);
			$pid=(int)$tmp['personen_id'];
			$id=$cid.'_'.$pid;

			if(isset($GLOBALS['_persicar'][$id]) && ($GLOBALS['_persicar'][$id]['mxh'] == $mxh))
				return $GLOBALS['_persicar'][$id];

			$fname=trim($tmp['pname']);
			$sname=trim($tmp['vorname']);
			$sex=(int)$tmp['geschlecht'];
			$ptype=(int)$tmp['personentyp'];
			$parent=(int)$tmp['elter'];
			if($tmp['inaktiv'])
				$inact=true;
			$vfrom=$tmp['gueltig_von'];
			$vto=$tmp['gueltig_bis'];
		}

		if(!isset($GLOBALS['_persicar'][$id]))
		{
			$GLOBALS['_persicar'][$id]=array(
				'pid'=>$pid, 'type'=>$ptype, 'parent'=>$parent, 'sex'=>$sex, 'inact'=>$inact,
				'fname'=>$fname, 'sname'=>$sname, 'name'=>$fname,
				'file'=>convertFilename($fname.$sname), 'mxh'=>$mxh,
				'def'=>true, 'mtime'=>'?',
				'src'=>'', '256'=>'', '128'=>'', '100'=>'', 'ico'=>'',
				'size_src'=>'', 'size_256'=>'', 'size_128'=>'', 'size_100'=>'', 'size_ico'=>'',
				'dsp'=>'', 'size_dsp'=>''
			);
			if(strlen($sname))
				$GLOBALS['_persicar'][$id]['name'] .= ', '.$sname;

			// New format (PNG)
			$npd=$GLOBALS['datpath'].$cid.'/PER/';
			$npico=$npd.$pid.'_ico.png';
			$np100=$npd.$pid.'_100.png';
			$np128=$npd.$pid.'_128.png';
			$np256=$npd.$pid.'_256.png';
			$npsrc=$npd.$pid.'_src.png';
//echo('$sex='.$sex.', $ptype='.$ptype.'<hr />$npico='.$npico.', $np100='.$np100.', $np128='.$np128.', $np256='.$np256.', $npsrc='.$npsrc.'<hr />');

			// Exists?
			if(file_exists($npico) || file_exists($np100) || file_exists($np128) || file_exists($np256) || file_exists($npsrc)) // yes
			{
				$GLOBALS['_persicar'][$id]['def']=false;

				if(file_exists($npico))
					$GLOBALS['_persicar'][$id]['ico']=$npico;
				if(file_exists($np100))
					$GLOBALS['_persicar'][$id]['100']=$np100;
				if(file_exists($np128))
					$GLOBALS['_persicar'][$id]['128']=$np128;
				if(file_exists($np256))
					$GLOBALS['_persicar'][$id]['256']=$np256;
				if(file_exists($npsrc))
					$GLOBALS['_persicar'][$id]['src']=$npsrc;
			}
			// Default
			else if($sex == FXP_GT_MALE)	// ...male
			{
				$ici='usm';
				$GLOBALS['_persicar'][$id]['100']=$GLOBALS['gfxpath'].$ici.'_b_100x100.png';
				$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['gfxpath'].$ici.'_b_20x20.png';
			}
			else if($sex == FXP_GT_MALE)	// ...female
			{
				$ici='usf';
				$GLOBALS['_persicar'][$id]['100']=$GLOBALS['gfxpath'].$ici.'_b_100x100.png';
				$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['gfxpath'].$ici.'_b_20x20.png';
			}
			else if($sex == FXP_GT_DIVERSE)	// ...diverse
			{
				$ici='tmf';
				$GLOBALS['_persicar'][$id]['100']=$GLOBALS['gfxpath'].$ici.'_b_100x100.png';
				$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['gfxpath'].$ici.'_b_20x20.png';
			}
			else if($sex == 99)				// ...admin
			{
				$ici='cst';
				$GLOBALS['_persicar'][$id]['100']=$GLOBALS['gfxpath'].$ici.'_b_100x100.png';
				$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['gfxpath'].$ici.'_b_20x20.png';
			}
			else if($ptype > 0)
			{
				$tia=array(FXP_PT_CLIENT=>'clt', FXP_PT_CLIENT_CP=>'adm', FXP_PT_CUSTOMER=>'cmp', FXP_PT_CUSTOMER_CP=>'cst', FXP_PT_PARTNER=>'prs', FXP_PT_PARTNER_CP=>'par', FXP_PT_EMPLOYEE=>'usm', FXP_PT_CONTRACTOR=>'ctr', FXP_PT_CONTACT=>'ctc', FXP_PT_CONTACT_CP=>'con');
				if(isset($tia[$ptype]))
				{
					$ici=$tia[$ptype];
					if(($ici == 'adm') || ($ici == 'con') || ($ici == 'cst') || ($ici == 'ctf') || ($ici == 'emm') || ($ici == 'par') || ($ici == 'usm'))	// Select male or female icons randomly
					{
						$r=rand(1,1000);
						if($r > 500)
							$ici=substr($ici,0,2).'f';
					}
				}
				else
					$ici='tmf';
				$GLOBALS['_persicar'][$id]['100']=$GLOBALS['gfxpath'].$ici.'_b_100x100.png';
				$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['gfxpath'].$ici.'_b_20x20.png';
			}
			else
			{
				$tia=array(164=>'adm', 'prs', 'clt', 'emm', 'ctr', 'cmp', 'cst', 'ctc', 'con', 'prs', 'par');
				if(isset($tia[$GLOBALS['fxpglobals']['tr']]))
				{
					$ici=$tia[$GLOBALS['fxpglobals']['tr']];
					if(($ici == 'adm') || ($ici == 'con') || ($ici == 'cst') || ($ici == 'ctf') || ($ici == 'emm') || ($ici == 'par') || ($ici == 'usm'))	// Select male or female icons randomly
					{
						$r=rand(1,1000);
						if($r > 500)
							$ici=substr($ici,0,2).'f';
					}
				}
				else
					$ici='tmf';
				$GLOBALS['_persicar'][$id]['100']=$GLOBALS['gfxpath'].$ici.'_b_100x100.png';
				$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['gfxpath'].$ici.'_b_20x20.png';
			}

			if(!strlen($GLOBALS['_persicar'][$id]['ico']))
			{
				if(strlen($GLOBALS['_persicar'][$id]['100']))
					$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['_persicar'][$id]['100'];
				else if(strlen($GLOBALS['_persicar'][$id]['128']))
					$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['_persicar'][$id]['128'];
				else if(strlen($GLOBALS['_persicar'][$id]['256']))
					$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['_persicar'][$id]['256'];
				else if(strlen($GLOBALS['_persicar'][$id]['src']))
					$GLOBALS['_persicar'][$id]['ico']=$GLOBALS['_persicar'][$id]['src'];
			}
		}
	}
	else if($GLOBALS['_persicar'][$id]['mxh'] == $mxh)
		return $GLOBALS['_persicar'][$id];

	// Sizes
	$sa=array('ico', '100', '128', '256', 'src');
	$ma=array();
	foreach($sa as $st)
	{
		if(strlen($GLOBALS['_persicar'][$id][$st]))
		{
			$ims=GetImageSize($GLOBALS['_persicar'][$id][$st]);
//if(($pid == 4598) || ($pid == 4697)) fxDebug($ims,$GLOBALS['_persicar'][$id][$st].' -- $mxh='.$mxh);
			if(fxIsArray($ims))
			{
				$w=$ims[0];
				$h=$ims[1];
				$sz=' width='.$w.' height='.$h;

				if($mxh && ($h != $mxh))
				{
					$w=round(($mxh*$w)/$h);
					$h=$mxh;
				}
				if(($st != 'ico') && $fit)
					$sz=' width='.$w.' height='.$h;

				$ma[$st]=array('dh'=>abs($mxh-$ims[1]), 'df'=>$ims[1]/$h, 'ow'=>$ims[0], 'oh'=>$ims[1], 'nw'=>$w, 'nh'=>$h, 'sz'=>$sz);
			}
			else
				$sz=' width=20 height=20';

			$GLOBALS['_persicar'][$id]['size_'.$st]=$sz;
		}
	}
//if(($pid == 4598) || ($pid == 4697)) fxDebug($ma,'$ma');

	if(fxIsArray($ma))
	{
		if(isset($ma['256']))	// Find closest match
		{
			$dh=1048576;
			foreach($ma as $st => $sa)
			{
				if(!strlen($ds) || ($sa['dh'] < $dh))
				{
					$dh=$sa['dh'];
					$ds=$st;
					$GLOBALS['_persicar'][$id]['dsp']=$GLOBALS['_persicar'][$id][$ds];

					if($fit)
						$GLOBALS['_persicar'][$id]['size_dsp']=' width='.$sa['nw'].' height='.$sa['nh'];
					else
						$GLOBALS['_persicar'][$id]['size_dsp']=$GLOBALS['_persicar'][$id]['size_'.$ds];
				}
			}
		}
		else					// Find biggest
		{
			$dh=0;
			foreach($ma as $st => $sa)
			{
				if(!strlen($ds) || ($sa['oh'] > $dh))
				{
					$dh=$sa['oh'];
					$ds=$st;
					$GLOBALS['_persicar'][$id]['dsp']=$GLOBALS['_persicar'][$id][$ds];

					if($fit)
						$GLOBALS['_persicar'][$id]['size_dsp']=' width='.$sa['nw'].' height='.$sa['nh'];
					else
						$GLOBALS['_persicar'][$id]['size_dsp']=$GLOBALS['_persicar'][$id]['size_'.$ds];
				}
			}
		}
	}

	// Last modify time
	if(strlen($GLOBALS['_persicar'][$id]['ico']))
		$GLOBALS['_persicar'][$id]['mtime'] .= filemtime($GLOBALS['_persicar'][$id]['ico']);

//if(($pid == 4598) || ($pid == 4697)) fxDebug($GLOBALS['_persicar'][$id],'$GLOBALS[\'_persicar\']['.$id.']');
	return $GLOBALS['_persicar'][$id];
}

/**
 * Replace special report generator alias fields
 *
 * @param boolean $values - Optional parameter (default = true):  If true the value for each field is set, else the value is set to true
 *
 * @return Array of special report generator alias fields
 */
function fxf_getRGSpecialFields($value=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$v=true;
	if($value)
		$v='$_psid';

	$a=array(
		'ansprechpartner'=>'$_psid',
		'elter_person'=>'$_psid',
		'hat_bestaetigt'=>'$_psid',
		'id_sprache'=>'$_spid',
		'kunde'=>'$_psid',
		'mutterprojekt_id'=>'$_prid',
		'person_angelegt'=>'$_psid',
		'person_zustaendig'=>'$_psid',
		'personen_id'=>'$_psid',
		'projekt_id'=>'$_prid',
		'projektleiter'=>'$_psid',
		'vertreter'=>'$_psid',
		'zkunde'=>'$_psid'
	);

	if(!$value)
	{
		foreach($a as $ac => $av)
			$a[$ac]=true;
	}

	return $a;
}
?>