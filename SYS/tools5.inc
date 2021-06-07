<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tools5.inc                                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-11-02                                                   //
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
 * PERSONENTYP_RECHTE
 *
 * @param boolean $asarray - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function personentyp_rechte($asarray=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ptypr='';
	// Prüfen, ob Benutzer Rechte hat auf...

	// ...Transaktion 63: "Personen->Mandanten->Personen einzeln->Kommunikation"
	// ...Transaktion 166: "Personen->Mandanten->Personen kompakt->Mandantendaten"
	if(tr_rechte(63) || tr_rechte(166))
		$ptypr .= ',274';
	// ...Transaktion 65: "Personen->Mandanten->Personen einzeln->Ansprechpartner"
	// ...Transaktion 164: "Personen->Mandanten->Personen kompakt->Ansprechpartner"
	if(tr_rechte(65) || tr_rechte(164))
		$ptypr .= ',2090';

	// ...Transaktion 69: "Personen->Kunden->Personen einzeln->Kommunikation"
	// ...Transaktion 169: "Personen->Kunden->Personen kompakt->Kundendaten"
	if(tr_rechte(69) || tr_rechte(169))
		$ptypr .= ',276';
	// ...Transaktion 72: "Personen->Kunden->Personen einzeln->Ansprechpartner->Kommunikation"
	// ...Transaktion 170: "Personen->Kunden->Personen kompakt->Ansprechpartner"
	if(tr_rechte(72) || tr_rechte(170))
		$ptypr .= ',620';

	// ...Transaktion 133: "Personen->Lieferanten->Personen einzeln->Kommunikation"
	// ...Transaktion 173: "Personen->Lieferanten->Personen kompakt->Lieferantendaten"
	if(tr_rechte(133) || tr_rechte(173))
		$ptypr .= ',2091';
	// ...Transaktion 132: "Personen->Lieferanten->Personen einzeln->Ansprechpartner->Kommunikation"
	// ...Transaktion 174: "Personen->Lieferanten->Personen kompakt->Ansprechpartner"
	if(tr_rechte(132) || tr_rechte(174))
		$ptypr .= ',2092';

	// ...Transaktion 70: "Personen->Mitarbeiter->Int. Mitarbeiter->Personen einzeln->Kommunikation"
	// ...Transaktion 167: "Personen->Mitarbeiter->Int. Mitarbeiter->Personen kompakt->Mitarbeiterdaten"
	if(tr_rechte(70) || tr_rechte(167))
		$ptypr .= ',275';

	// ...Transaktion 71: "Personen->Mitarbeiter->Ext. Mitarbeiter->Personen einzeln->Kommunikation"
	// ...Transaktion 168: "Personen->Mitarbeiter->Ext. Mitarbeiter->Personen kompakt->Mitarbeiterdaten"
	if(tr_rechte(71) || tr_rechte(168))
		$ptypr .= ',840';

	// ...Transaktion 96: "Personen->CRM->Kontake->Personen einzeln->Kommunikation"
	// ...Transaktion 171: "Personen->CRM->Kontake->Personen kompakt->Kontaktdaten"
	if(tr_rechte(96) || tr_rechte(171))
		$ptypr .= ',1372';
	// ...Transaktion 125: "Personen->CRM->Kontake->Personen einzeln->Ansprechpartner->Kommunikation"
	// ...Transaktion 172: "Personen->CRM->Kontake->Personen kompakt->Ansprechpartner"
	if(tr_rechte(125) || tr_rechte(172))
		$ptypr .= ',841';

	if(strlen($ptypr))
		$ptypr=substr($ptypr, 1);
	else
		$ptypr='0';

	// Werte als Array zurückgeben
	if($asarray)
	{
		$ptr= explode(',', $ptypr);
		$ptypr= '';

		foreach($ptr as $p)
			$ptypr[$p]=true;
	}

	return $ptypr;
}

/**
 * Funktion prüft, ob ein übergebener String eine gültige Telefonnumer ist
 * Rückgabe: true, falls gültige Telefonnummer, sonst false
 *
 * @param various $pn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function is_phone($pn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($pn))
		return false;

	for($i=0; $i<strlen($pn); $i++)
	{
		$fc=substr($pn, $i, 1);
		if((ord($fc) >= 0x30) && (ord($fc) <= 0x39))
			continue;
		if(($fc != '+') && ($fc != '-') && ($fc != '(') && ($fc != ')') && ($fc != '/')  && ($fc != ' '))
			return false;
	}

	return true;
}

/**
 * Speichern der User-Settings für diesen User zur Transaktion in der Tabelle "teinstellung"
 * Die Einstellungen, gespeichert in $settings werden mit der Funktion "retrieve_settings"
 * aus der Datenbank ausgelesen. Die settings sollten am besten so formatiert werden, dass
 * sie per "eval()" ausgeführt werden können
 *
 * @param various $settings - Mandatory parameter:                ???PARAMETER???
 * @param various $s_tr     - Mandatory parameter:                ???PARAMETER???
 * @param string  $user     - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $client   - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function store_settings($settings, $s_tr, $user='', $client='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($user))
		$user=$GLOBALS['fxpglobals']['user'];
	if(!strlen($client))
		$client=$GLOBALS['fxpglobals']['client'];
	$settings=convert_string($settings, 'todb');

	// Bereits eine Einstellung gespeichert
	$set_exists=db_value("SELECT trans_id FROM teinstellung WHERE trans_id=".$s_tr." AND benutzer_id=".$user." AND mandanten_id=".$client);

	// JA, dann Update
	if($set_exists)
		$sql="UPDATE teinstellung SET beschreibung='".$settings."' WHERE trans_id=".$s_tr." AND benutzer_id=".$user." AND mandanten_id=".$client;
	// NEIN, dann Insert
	else
		$sql="INSERT INTO teinstellung (mandanten_id, trans_id, benutzer_id, beschreibung) VALUES (".$client.", ".$s_tr.", ".$user.", '".$settings."')";
	$err=db_x($sql);
//fxDebug($err,$sql);

	return $err;
}

/**
 * Liest Settings von einem User zu einer Transaktion aus der Datenbank
 *
 * @param various $s_tr   - Mandatory parameter:                ???PARAMETER???
 * @param string  $user   - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $client - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function retrieve_settings($s_tr, $user='', $client='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($user))
		$user=$GLOBALS['fxpglobals']['user'];
	if(!strlen($client))
		$client=$GLOBALS['fxpglobals']['client'];

	$setting=db_value("SELECT beschreibung FROM teinstellung WHERE trans_id=".$s_tr." AND benutzer_id=".$user." AND mandanten_id=".$client);

	return $setting;
}

/**
 * Löscht alle Settings eines ganzen Mandanten zu einer Transaktion aus der Datenbank
 *
 * @param various $s_tr   - Mandatory parameter:                ???PARAMETER???
 * @param string  $user   - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $client - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function delete_settings($s_tr, $user='', $client='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($user))
		$user=$GLOBALS['fxpglobals']['user'];
	if(!strlen($client))
		$client=$GLOBALS['fxpglobals']['client'];

	$err=false;

	$sql="SELECT aenderungs_id FROM teinstellung WHERE trans_id=".$s_tr." AND benutzer_id=".$user." AND mandanten_id=".$client;
	$bex=db_value($sql);
	if($bex)
	{
		$sql="DELETE FROM teinstellung WHERE trans_id=".$s_tr." AND benutzer_id=".$user." AND mandanten_id=".$client;
		$err=db_x($sql);
//fxDebug($err,$sql);
	}

	return $err;
}

/**
 * Formatiert microtime Wert in Wert mit dem man rechnen kann
 *
 * @return ???RETURN???
 */
function format_micro()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mt=microtime();

	return substr($mt, 11).substr($mt, 1, 9);
}

/**
 * Mandantenmenüdateien löschen
 *
 * @param boolean $doit       - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $onlyclient - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function deleteClientMenuFiles($doit=true, $onlyclient=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Directory einlesen und nach Menu-Include-Dateien filtern
	$menudir=$GLOBALS['datpath'];
	$directory=array();
	$dcnt=0;
	$dir_handle=opendir($menudir); // Directory öffnen
	$cpart=substr('0000'.$GLOBALS['fxpglobals']['client'],-4);
	while(false !== ($file=readdir($dir_handle)))
	{
		if(($file == '.') || ($file == '..'))
			continue;
		if(is_dir($menudir.$file))						// Sub-Directories überspringen
			continue;
		if(strtolower(substr($file, 0, 3)) !== 'cm-')	// Alle Nicht-Menu-Files überspringen
			continue;
		if(!stristr($file, '.inc') !== false)			// Keine Include-Files ebenfalls ignorieren
			continue;

		// Alle Nicht-Mandantenmenüdateien überspringen, falls angegeben
		if($onlyclient && (strtolower(substr($file, 3, 4)) !== $cpart))
			continue;

		$directory[$dcnt++]=$menudir.$file;
	}

	if(!$doit)
		return $directory;

	if(sizeof($directory))
	{
		for($i=0; $i<sizeof($directory); $i++)
		{
			@chmod($directory[$i], 0777);
			@unlink($directory[$i]);
		}
	}
}

/**
 * Masken-Dateien löschen
 *
 * @param string $pattern - Optional parameter (default = ''): ???PARAMETER???
 */
function deleteMaskFiles($pattern='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$filedir=$GLOBALS['micpath'];
	$plen=strlen($pattern);

	// Directory einlesen und alle Dateien löschen
	$dir_handle=opendir($filedir); // Directory öffnen
	while(false !== ($file=readdir($dir_handle)))
	{
		if(($file == '.') || ($file == '..'))
			continue;
		if(is_dir($filedir.$GLOBALS['_oss'].$file))		// Sub-Directories überspringen
			continue;
		if($file == 'index.php')						// index.php überspringen
			continue;

		if(!$plen || (substr($file, 0, $plen) == $pattern))
		{
			@chmod($filedir.$GLOBALS['_oss'].$file, 0777);
			@unlink($filedir.$GLOBALS['_oss'].$file);
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $feldname - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function check_replacevalue($feldname)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($feldname))
		return false;

	$feldname=strtolower($feldname);

	if(is_array($GLOBALS['db_values']) && is_array($GLOBALS['db_values'][$feldname]))
	{
		$feldname_ok= $GLOBALS['db_values'][$feldname]['feldname_ok'];
		$nachschlage= $GLOBALS['db_values'][$feldname]['nachschlage'];
		$feldtyp= $GLOBALS['db_values'][$feldname]['feldtyp'];
	}
	else
	{
		$sql="SELECT feldtyp, alias_zu, nachschlage FROM felder WHERE ".db_like('feldname', $feldname)." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
		$tmp=db_values($sql);
//fxDebug($tmp, $sql);
		if(is_array($tmp))
		{
			$feldname_ok= $feldname;
			$nachschlage= (int)$tmp['nachschlage'];
			$alias= (int)$tmp['alias_zu'];
			$feldtyp= (int)$tmp['feldtyp'];

			if($alias)
			{
				$sql="SELECT feldname, feldtyp, nachschlage FROM felder WHERE id=".$alias." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
				$tmp=db_values($sql);
//fxDebug($tmp, $sql);
				if(is_array($tmp))
				{
					$feldname_ok= strtolower($tmp['feldname']);
					$nachschlage= (int)$tmp['nachschlage'];
					$feldtyp= (int)$tmp['feldtyp'];
				}
			}
		}
		if(!is_array($GLOBALS['db_values']) || !is_array($GLOBALS['db_values'][$feldname]))
			$GLOBALS['db_values'][$feldname]=array('feldname_ok'=>$feldname_ok, 'nachschlage'=>$nachschlage, 'feldtyp'=>$feldtyp);
		if(!is_array($GLOBALS['db_values']) || !is_array($GLOBALS['db_values'][$feldname_ok]))
			$GLOBALS['db_values'][$feldname_ok]=array('feldname_ok'=>$feldname_ok, 'nachschlage'=>$nachschlage, 'feldtyp'=>$feldtyp);
	}
//fxDebug($GLOBALS['db_values'][$feldname_ok], $feldname.' -> '.$feldname_ok.'='.$id);

	switch($feldname_ok)
	{
		case 'vorgangsnummer':
		case 'projekt_id':
		case 'personen_id':
		case 'personen_id_alias':
		case 'benutzer_id':
			return true;
		break;

		default:
			return false;
	}
}

/**
 * Liefert anhand eines Feldnamens und einer ID den aufgeschlüsselten Wert aus der Datenbank,
 * z.B. Feldname="Personen_ID" + ID="1086" liefert "Binder, Walter"
 *
 * @param various $feldname - Mandatory parameter:                ???PARAMETER???
 * @param various $id       - Mandatory parameter:                ???PARAMETER???
 * @param string  $pre      - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_db_value($feldname, $id, $pre='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($feldname) || !strlen($id))
		return false;

	$feldname=strtolower($feldname);
	$rvalue=check_replacevalue($feldname);

	$feldname_ok= $GLOBALS['db_values'][$feldname]['feldname_ok'];
	$nachschlage= $GLOBALS['db_values'][$feldname]['nachschlage'];
	$feldtyp= $GLOBALS['db_values'][$feldname]['feldtyp'];

	if(is_array($GLOBALS['db_values']) && is_array($GLOBALS['db_values'][$feldname_ok]) && isset($GLOBALS['db_values'][$feldname_ok][$id]))
		return $GLOBALS['db_values'][$feldname_ok][$id];

	if($nachschlage)
		$rval=meldung($id);
	else
	{
		$rval= '';
		$intid= (int)$id;
		switch($feldname_ok)
		{
			case 'vorgangsnummer':
				$rval=no_null($id);
			break;

			case 'projekt_id':
				if($intid)
				{
					$valarr=db_values("SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$intid." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")");
					if(is_array($valarr))
					{
						// nach Vorgangsnummer sortieren
						if(strtolower($GLOBALS['fxpglobals']['settings']['zeit_projektnr']) == 'ja')
							$rval='<b>'.no_null($valarr['vorgangsnummer']).'</b>: '.$valarr['name_projekt'];
						else
							$rval='<b>'.$valarr['name_projekt'].'</b> ('.no_null($valarr['vorgangsnummer']).')';
					}
				}
			break;

			case 'personen_id':
			case 'personen_id_alias':
				if($intid)
				{
					$sql  = "SELECT pname, vorname FROM personen WHERE personen_id=".$intid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp =db_values($sql);
					if(fxIsArray($tmp))
					{
						$rval=trim($tmp['pname']);
						if(strlen($tmp['vorname']))
							$rval .= ',&nbsp;'.trim($tmp['vorname']);
					}
				}
			break;

			case 'benutzer_id':
				if($intid)
				{
					$sql  = "SELECT b.name_benutzer, p.pname, p.vorname FROM benutzer b, personen p WHERE b.benutzer_id=".$intid." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$sql .= " AND p.personen_id=b.personen_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$tmp =db_values($sql);
					if(fxIsArray($tmp))
					{
						$rval=trim($tmp['name_benutzer']).'&nbsp;<i>('.trim($tmp['pname']);
						if(strlen($tmp['vorname']))
							$rval .= ',&nbsp;'.trim($tmp['vorname']);
						$rval .= ')</i>';
					}
				}
			break;

			default:
				if(($feldtyp == 1) || ($feldtyp == 3) || ($feldtyp == 15))	// Textfelder
					return $pre.'&nbsp;'.$id.'&nbsp';
				return false;
		}
	}

	if(!strlen($rval))
		return false;

	$rval=$pre.'&nbsp;'.$rval.'&nbsp;';
	$GLOBALS['db_values'][$feldname_ok][$id]=$rval;

	return $rval;
}

/**
 * ???FUNCTION???
 *
 * @param various $trans - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpLink($trans)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$trans=(int)$trans;
	if(tr_rechte($trans))
	{
		$lnk='';

		// Load Menu and extract transaction link and path
		$ma=loadMenu(true);
		if(fxIsArray($ma))
		{
			foreach($ma as $tk => $tc)
			{
				if($tc['tr'] == $trans)
				{
					$arg='';
					if(strlen($tc['ar']))
						$arg=' arg="'.$tc['ar'].'"';
					$id='id="fxm_'.substr($tk,1).'_l" tr="'.$tc['tr'].'"'.$arg;
					$lnk=str_replace('{id}',$id,'<b {id} style="cursor:pointer;">'.$tc['tx'].'</b>');
					while(strlen($tk) > 3)
					{
						$tk=substr($tk,0,-3);
						$id='id="fxm_'.substr($tk,1).'_l"';
						$lnk=str_replace('{id}',$id,'<u {id} class="grey" style="cursor:pointer;">'.$ma[$tk]['tx'].'</u>').' <font style="color:#cccccc;">&gt;</font> '.$lnk;
					}
					break;
				}
			}
		}

		if(!strlen($lnk))
			$lnk='TR '.$trans;

		$lnk='<span style="border:1px solid #aaaaaa;border-radius:6px;padding:4px 8px;background:#e8e8e8;">'.$lnk.'</span>';
	}
	else
	{
		$lnk='';
	}

	return $lnk;
}

/**
 * ???FUNCTION???
 *
 * @param integer $pid       - Optional parameter (default = 0):  ???PARAMETER???
 * @param string  $datum     - Optional parameter (default = ''): ???PARAMETER???
 * @param integer $monatlich - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function summe_zeitkonto($pid=0, $datum='', $monatlich=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$summe_zeitkonto_debug=0;

	if(!$pid)
		$pid=$GLOBALS['fxpglobals']['person'];

	if(!strlen($datum))
		$datum=substr($GLOBALS['datetime'],0,6).'01000000';
	else
		$datum=substr($datum.'00000000',0,8).'000000';
	$mdatum=substr($datum,0,6).'00000000';

	$merster=0;
	if(substr($datum,6,2) == '01')
		$merster=1;

	if($monatlich)
		$monatlich=1;
	else
		$monatlich=0;

	if($summe_zeitkonto_debug)
		echo('('.__LINE__.') <b>Startbedingungen:</b><br />-> $pid: '.$pid.', $datum: '.$datum.', $monatlich: '.$monatlich.' -&gt; $mdatum='.$mdatum.', $merster='.$merster.'<br />');

	$ej=(int)substr($mdatum,0,4);
	$em=(int)substr($mdatum,4,2);

	$lj=$ej;
	$lm=$em-1;
	if(!$lm)
	{
		$lj--;
		$lm=12;
	}

	// Bilanz des letzten Monats aus der Datenbank holen
	if(!$monatlich)
	{
		$ldat=substr('0000'.$lj, -4).substr('00'.$lm, -2).'00000000';
		$sql="SELECT zeitaufwand FROM zeitdaten WHERE zeitart IN (104,105) AND personen_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND ist_beginn='".$ldat."'";
		$lbil=db_value($sql);
		if($summe_zeitkonto_debug)
		{
			echo('('.__LINE__.') <b>Bilanz des letzten Monats [$lbil] aus der Datenbank holen:</b><br />');
			fxDebug($lbil, $sql.'<br />= '.get_entry($lbil/3600,'dezimal').' h');
		}
		// Bilanz gefunden, dann zurückliefern falls Monatserster
		if(strlen($lbil) && $merster)
			return $lbil;
	}

	// Personendaten ermitteln
	$summe=0;
	$heute=substr($GLOBALS['datetime'],0,8);
	$sdatum='';
	$eanzeige='';
	$soll_stunden_tag=0;

	$mand_id=$GLOBALS['fxpglobals']['client'];

	$sql  = "SELECT mandanten_id, maeintrittsdatum, maaustrittsdatum, wochenarbeitstd, arbeitstage, montag AS a, dienstag AS b, mittwoch AS c, donnerstag AS d, freitag AS e, samstag AS f, sonntag AS g";
	$sql .= " FROM madaten WHERE personen_id=".$pid." AND mandanten_id=".$mand_id;
	$pdat=db_values($sql);
	if($summe_zeitkonto_debug)
	{
		echo('('.__LINE__.') <b>Personendaten ermitteln:</b><br />');
		fxDebug($pdat,$sql);
	}
	if(fxIsArray($pdat))
	{
		$maeintritt=substr($pdat['maeintrittsdatum'],0,8);
		$maaustritt=substr($pdat['maaustrittsdatum'],0,8);
		if(!strlen($maaustritt))
			$maaustritt='99999999';
		if($pdat['arbeitstage'])
			$soll_stunden_tag=($pdat['wochenarbeitstd']/$pdat['arbeitstage'])*3600;
	}
	else
	{
		$maeintritt=substr($mdatum,0,6).'01';
		$maaustritt='99999999';
	}
	$sanzeige=$maeintritt;
	$eanzeige=$maaustritt;
	if($summe_zeitkonto_debug)
		echo('('.__LINE__.') <b>Ermittelte Werte:</b><br />-> $sanzeige: '.$sanzeige.', $eanzeige: '.$eanzeige.' - $summe: '.$summe.'<br />');

	// Startbilanz holen
	$sql="SELECT zeitaufwand, ist_beginn FROM zeitdaten WHERE zeitart=105 AND personen_id=".$pid." AND mandanten_id=".$mand_id;
	$sbil=db_values($sql);
	if($summe_zeitkonto_debug)
	{
		echo('('.__LINE__.') <b>Startbilanz [$sbil] holen:</b><br />');
		fxDebug($sbil,$sql.'<br />= '.get_entry($sbil['zeitaufwand']/3600,'dezimal').' h');
	}
	if(fxIsArray($sbil))
	{
		$sj=substr($sbil['ist_beginn'],0,4);
		$sm=substr($sbil['ist_beginn'],4,2);
		$sm++;
		if($sm > 12)
		{
			$sm=1;
			$sj++;
		}
		$sanzeige=substr('0000'.$sj, -4).substr('00'.$sm, -2).'01';
		$sdatum=substr('0000'.$sj, -4).substr('00'.$sm, -2);
		if((substr($sbil['ist_beginn'], 0, 6).'00000000') < $mdatum)
			$summe=$sbil['zeitaufwand'];
	}
	else if($GLOBALS['fxpglobals']['settings']['zk_ueb_jahr'])
	{
		$seanzeige=substr('0000'.$GLOBALS['fxpglobals']['settings']['zk_ueb_jahr'], -4). substr('00'.$GLOBALS['fxpglobals']['settings']['zk_ueb_monat'], -2).'01';
		if($seanzeige > $sanzeige)
			$sanzeige=$seanzeige;
	}
	if($summe_zeitkonto_debug)
		echo('('.__LINE__.') <b>Ermittelte Werte:</b><br />-> $sanzeige: '.$sanzeige.', $eanzeige: '.$eanzeige.' - $sdatum: '.$sdatum.', $summe: '.$summe.' (= '.get_entry($summe/3600,'dezimal').')<br />');

	// Letzte eingetragene Bilanz aus der Datenbank holen
	if(!$monatlich)
	{
		$sql="SELECT zeitaufwand, ist_beginn FROM zeitdaten WHERE zeitart=104 AND personen_id=".$pid." AND mandanten_id=".$mand_id." AND ist_beginn<'".$mdatum."'";
		if(strlen($sdatum))
			$sql .= " AND ist_beginn>='".$sdatum."00000000'";
		$sql .= " ORDER BY ist_beginn DESC";
		$mbil=db_values($sql);
		if($summe_zeitkonto_debug)
		{
			echo('('.__LINE__.') <b>Letzte eingetragene Bilanz [$mbil] aus der Datenbank holen:</b><br />');
			fxDebug($mbil,$sql.'<br />= '.get_entry($mbil['zeitaufwand']/3600,'dezimal').' h');
		}
		if(fxIsArray($mbil))
		{
			$sj=(int)substr($mbil['ist_beginn'],0,4);
			$sm=(int)substr($mbil['ist_beginn'],4,2);
			$sm++;
			if($sm > 12)
			{
				$sm=1;
				$sj++;
			}
			$sanzeige=substr('0000'.$sj, -4).substr('00'.$sm, -2).'01';
			$sdatum=$sj.substr('00'.$sm, -2);
			$summe=$mbil['zeitaufwand'];
		}
		if($summe_zeitkonto_debug)
			echo('('.__LINE__.') <b>Ermittelte Werte:</b><br />-> $sanzeige: '.$sanzeige.', $eanzeige: '.$eanzeige.' - $summe: '.$summe.' (= '.get_entry($summe/3600,'dezimal').')<br />');
	}

	$edatum=substr($datum,0,8);
	$edatum6=substr($edatum,0,6);

	$sj=(int)substr($sanzeige,0,4);
	$sm=(int)substr($sanzeige,4,2);
	if(!$sdatum)
		$sdatum6=substr($sanzeige,0,6);
	else
		$sdatum6=substr($sdatum,0,6);
	$sdatum=$sdatum6.'01';

	if($summe_zeitkonto_debug)
		echo('('.__LINE__.') <b>Ermittelte Werte:</b><br />-> $sdatum: '.$sdatum.', $edatum: '.$edatum.'<br />');

	if($sdatum < $edatum)
	{
		// Alle Zeiteinträge in der Zeitspanne ermitteln
		$sql  = "SELECT zeitart, zeitaufwand, ist_beginn, projekt_id FROM zeitdaten WHERE zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND personen_id=".$pid;
		$sql .= " AND mandanten_id=".$mand_id." AND (ist_beginn<'".$edatum."000000' AND ist_beginn>='".$sdatum."000000') ORDER BY ist_beginn";
		$mbil=db_values($sql, '*');
		if($summe_zeitkonto_debug)
		{
			echo('('.__LINE__.') <b>Alle Zeiteinträge in der Zeitspanne ermitteln:</b><br />');
			fxDebug($mbil, $sql);
		}

		// Alle Überstundenauszahlungen ermitteln
		$oop=overtime_outpayment($pid, $sdatum,$edatum);
		if($summe_zeitkonto_debug)
		{
			echo('('.__LINE__.') <b>Alle Überstundenauszahlungen ermitteln:</b><br />');
			fxDebug($opa, 'overtime_outpayment('.$pid.', '.$sdatum.','.$edatum.')');
		}

		// Feier- und Sondertage ermitteln
		$feiertage=get_feiertage($sdatum, $edatum6.'31');
		$usk_tage=get_sondertage($sdatum, $edatum6.'31', $pid);
		if($summe_zeitkonto_debug)
		{
			echo('('.__LINE__.') <b>Feier- und Sondertage ermitteln:</b><br />');
			fxDebug($feiertage,'Feiertage');
			fxDebug($usk_tage,'Sondertage');
		}

		// Monatstage
		$mo_tage=array(1 => 31,28,31,30,31,30,31,31,30,31,30,31);
		$lj=fx_date('L', fx_mktime(0,0,0, 1,1,$sj));
		if($lj)
			$mo_tage[2]=29;

		// Startwochentag ermitteln
		$stg=fx_date('w', fx_mktime(0,0,0, $sm,1,$sj)) - 1;
		if($stg < 0)
			$stg += 7;

		if($summe_zeitkonto_debug)
			echo('('.__LINE__.') <b>Berechnung starten:</b><br />-> $sdatum: '.$sdatum.' ($sj: '.$sj.', $sm: '.$sm.') - $edatum: '.$edatum.' -- $summe: '.$summe.' (= '.get_entry($summe/3600,'dezimal').'), $soll_stunden_tag: '.$soll_stunden_tag.' (= '.get_entry($soll_stunden_tag/3600,'dezimal').'), $stg: '.$stg.'<br />');

		$bilanz=array();
		$kappbz=array();

		// Zwischenbilanzen ermitteln
		while($sdatum < $edatum)
		{
			if($summe_zeitkonto_debug)
				echo('('.__LINE__.') <b>Zeitraum:</b><br />-> $sdatum: '.$sdatum.' ($sj: '.$sj.', $sm: '.$sm.', tage: '.$mo_tage[$sm].')<br />');
			$ia=array();
			$ist=0;
			$soll=0;
			$ueausz=0;
			$atage=0;

			// Ist-Stunden ermitteln
			// ÄNDERUNG 11.01.2013: Nur Ist-Stunden bis einschl. heute zählen
			// ÄNDERUNG 30.10.2014: Alle Ist-Stunden nun wieder zählen
			if(fxIsArray($mbil))
			{
				foreach($mbil as $mba)
				{
					$dbm=substr($mba['ist_beginn'],0,8);
					$mbm=substr($dbm,0,6);
					if($mbm > $sdatum6)
						break;
					else if(($mbm < $sdatum6) || ($mba['projekt_id'] < 0))
						continue;

					$ist += $mba['zeitaufwand'];
					$ia[$dbm]=$mba['zeitaufwand']/3600;
				}
			}
			if($summe_zeitkonto_debug)
				echo('('.__LINE__.') <b>Ist-Stunden ermitteln:</b><br />-> $ist: '.$ist.' (= '.get_entry($ist/3600,'dezimal').')<br />');
//fxDebug($ia,'$ia');

			// Soll-Stunden ermitteln
			$kok=true;
			for($tag=1; $tag<=$mo_tage[$sm]; $tag++)
			{
				if($summe_zeitkonto_debug)
					echo('<b>Tag</b> '.$tag.':<br />');
				$tagstr=$sdatum6.substr('00'.$tag, -2);
				if($tagstr >= $edatum)
				{
					$kok=false;
					break;
				}

				$wday=0;
				if($pdat[chr($stg + ord('a'))] == 1) // Arbeitstag
				{
					$wday=1;
					if(($tagstr <= $heute) && ($tagstr >= $sanzeige) && ($tagstr >= $maeintritt) && ($tagstr <= $eanzeige))
					{
						// Feiertag
						if($feiertage[$tagstr] && (($feiertage[$tagstr]['art'] == FXP_HL_FULL) || ($feiertage[$tagstr]['art'] == FXP_HL_HALF)))
						{
							if($summe_zeitkonto_debug)
								echo('-> <b>Feiertag:</b> ('.$feiertage[$tagstr]['art'].')<br />');
							if($feiertage[$tagstr]['art'] == FXP_HL_HALF) // Halber Feiertag
							{
								$wday=2;
								$soll += ($soll_stunden_tag / 2);
								$atage += 0.5;
							}
							else
								$wday=0;
						}

						// Sondertage, ausser Gleittag
						if($wday && fxIsArray($usk_tage[$tagstr]) && ($usk_tage[$tagstr][-1]['typ'] != FXP_AT_SLIDINGDAYREQUEST) && ($usk_tage[$tagstr][-1]['typ'] != FXP_AT_SLIDINGDAY))
						{
							$uskadd=(double)$usk_tage[$tagstr][-1]['dauer'];
							if($summe_zeitkonto_debug)
								echo('-> <b>Sondertag:</b> ('.$usk_tage[$tagstr][-1]['typ'].' ['.$uskadd.'])<br />');

							if($wday == 2)	// Halber Feiertag
							{
								$soll -= ($soll_stunden_tag / 2);
								$atage -= 0.5;
								$wday=0;
							}
							else if($uskadd == 0.5)
							{
								$soll += ($soll_stunden_tag / 2);
								$atage += 0.5;
								$wday=2;
							}
							else
								$wday=0;
						}
					}
				}

				if(($wday == 1) && (($tagstr < $heute) || isset($ia[$tagstr])) && ($tagstr >= $sanzeige) && ($tagstr >= $maeintritt) && ($tagstr <= $eanzeige))
				{
					$soll += $soll_stunden_tag;
					$atage++;
				}
				if($summe_zeitkonto_debug)
					echo('-> <b>$soll:</b> '.$soll.' (= '.get_entry($soll/3600,'dezimal').')<br />');

				if(isset($oop[$tagstr]))
				{
					$ueausz += $oop[$tagstr]['sec'];
					if($summe_zeitkonto_debug)
						echo('-> <b>$Überstundenauszahlung:</b> '.$ueausz.' (= '.get_entry($ueausz/3600,'dezimal').')<br />');
				}

				// Nächster Wochentag
				$stg++;
				if($stg > 6)
					$stg=0;
			}
			$uestd=(int)($ist - $soll);
			if($summe_zeitkonto_debug)
				echo('('.__LINE__.') <b>Überstunden:</b><br />-> $uestd: '.$uestd.' (= '.get_entry($uestd/3600,'dezimal').')<br />');

			$uestd=(int)($uestd - $ueausz);
			$summe=(int)($summe + $uestd);
			if($summe_zeitkonto_debug)
				echo('<hr />('.__LINE__.') <b>Monatssumme:</b><br />-> $ist: '.$ist.' (= '.get_entry($ist/3600,'dezimal').'), $soll: '.$soll.' (= '.get_entry($soll/3600,'dezimal').'), $ueausz: '.$ueausz.' (= '.get_entry($ueausz/3600,'dezimal').') = $summe: '.$summe.' (= '.get_entry($summe/3600,'dezimal').')<hr />');
			$bilanz[$sdatum6]=$summe;

			$sm++;
			if($sm > 12)
			{
				$sm=1;
				$sj++;
				$lj=fx_date('L', fx_mktime(0, 0, 0, 1, 1, $sj));
				if($lj)
					$mo_tage[2]=29;
				else
					$mo_tage[2]=28;
			}
			$sdatum=$sj.substr('00'.$sm,-2).'01';
			$sdatum6=substr($sdatum,0,6);
		}

		// Ermittele Zwischenbilanzen in die Datenbank schreiben
		// ÄNDERUNG 11.01.2013: nur falls Datumswert < Heute
		if(!$monatlich && $merster)
		{
			for($i=0; $i<2; $i++)
			{
				if($i)
				{
					$barr=$kappbz;
					$zart=2467;
				}
				else
				{
					$barr=$bilanz;
					$zart=104;
				}

				if(fxIsArray($barr))
				{
					if($summe_zeitkonto_debug)
					{
						echo('('.__LINE__.') <b>Ermittele Zwischenbilanzen in die Datenbank schreiben, falls Datum < Heute:</b><br />');
						fxDebug($barr, '$barr: Bilanz '.$i);
					}

					$h6=substr($heute,0,6);
					foreach($barr as $bdatum => $bsumme)
					{
						if($bdatum < $h6)
						{
							$ok=true;
							$sql="SELECT zeit_id, zeitaufwand FROM zeitdaten WHERE personen_id=".$pid." AND mandanten_id=".$mand_id." AND zeitart=".$zart." AND ist_beginn='".$bdatum."00000000'";
							$cbs=db_values($sql);
							if($summe_zeitkonto_debug > 1)
								fxDebug($cbs, $sql);
							if(fxIsArray($cbs) && ((int)$cbs['zeitaufwand'] == $bsumme))
								$ok=false;

							if($ok)
							{
								$Zeit_ID=id_max('zeitdaten', 'zeit_id');

								$zd=array
								(
									'mandanten_id'			=> $mand_id,
									'zuordnungs_mid'		=> $mand_id,
									'personen_id'			=> $pid,
									'zeit_id'				=> $Zeit_ID,
									'projekt_id'			=> 0,
									'mutterprojekt_id'		=> 0,
									'rechnungs_id'			=> 0,
									'positions_id'			=> 0,
									'zkunde'				=> 0,

									'soll_beginn'			=> 'NULL',
									'soll_ende'				=> 'NULL',
									'ist_beginn'			=> "'".$bdatum."00000000'",
									'ist_ende'				=> "'".$bdatum."00000000'",
									'kw'					=> 0,
									'pause'					=> 0,
									'zeitaufwand'			=> $bsumme,

									'kosten_ek'				=> 0,
									'kosten_vk'				=> 0,
									'kosten_mvk'			=> 0,
									'mwst_satz'				=> 0,

									'rk_ek'					=> 'NULL',
									'rk_vk'					=> 'NULL',
									'rk_ort1'				=> 'NULL',
									'rk_ort2'				=> 'NULL',
									'rk_km'					=> 'NULL',
									'rk_anzNacht'			=> 'NULL',
									'rk_kategorie'			=> 'NULL',

									'nicht_abrechenbar'		=> 'NULL',
									'rk_nicht_abrechenbar'	=> 'NULL',
									'soll_verzoeg'			=> 'NULL',
									'ist_verzoeg'			=> 'NULL',
									'zeiteinheit'			=> 'NULL',
									'verz_grund'			=> 'NULL',

									'zeitart'				=> $zart,
									'zeitgruppe'			=> 'NULL',
									'freigabe'				=> 0,
									'offline_zd'			=> 'NULL'
								);
								$zk="";
								$zv="";
								foreach($zd as $key => $value)
								{
									if(strlen($zk))
									{
										$zk .= ", ";
										$zv .= ", ";
									}
									$zk .= $key;
									$zv .= $value;
								}
								$query_zeit="INSERT INTO zeitdaten (".$zk.") VALUES (".$zv.")";
								if($summe_zeitkonto_debug > 1)
									fxDebug($query_zeit, '$query_zeit');
								$err=db_x($query_zeit);
								if($err)
									break 2;
							}
						}
					}
				}
			}
		}
	}

	// Ermittelte Bilanz zurückliefern
	if($monatlich)
		return $bilanz;
	return $summe;
}

/**
 * ???FUNCTION???
 *
 * @param integer $pid   - Optional parameter (default = 0):  ???PARAMETER???
 * @param string  $datum - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function bilanz_zeitkonto_loeschen($pid=0, $datum='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($datum))
		$datum=substr($GLOBALS['datetime'],0,6).'00000000';
	else
		$datum=substr($datum,0,6).'00000000';
	if(!$pid)
		$pid=$GLOBALS['fxpglobals']['person'];
	$mand_id=$GLOBALS['fxpglobals']['client'];
//echo("-> bilanz_zeitkonto_loeschen($pid, $datum)<br />");

	$err=false;

	// Prüfen, ob Bilanzen vorhanden
	$sql="SELECT zeit_id FROM zeitdaten WHERE personen_id=".$pid." AND mandanten_id=".$mand_id." AND zeitart IN (104,2467) AND ist_beginn>='".$datum."'";
	$chk=db_value($sql);

	// ...Ja, löschen
	if($chk)
	{
		$sql="DELETE FROM zeitdaten WHERE personen_id=".$pid." AND mandanten_id=".$mand_id." AND zeitart IN (104,2467) AND ist_beginn>='".$datum."'";
		$err=db_x($sql);
	}

	return $err;
}

/**
 * Veraltet
 *
 * @param boolean $reload - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_tr_menu($reload=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mn=loadMenu(false,$reload);
	return $mn;
}

/**
 * ???FUNCTION???
 *
 * @param string  $top    - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $inhalt - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $bottom - Optional parameter (default = ''): ???PARAMETER???
 * @param integer $width  - Optional parameter (default = 0):  ???PARAMETER???
 * @param integer $mnr    - Optional parameter (default = 0):  ???PARAMETER???
 */
function msk_window($top='', $inhalt='', $bottom='', $width=0, $mnr=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$width=(int)$width;
	if($width <= 0)
		$width=940;

	$mnr=(int)$mnr;
	if(!$mnr && isset($GLOBALS['_photo_masknumber']))
	{
		$mnr=(int)$GLOBALS['_photo_masknumber'];
		unset($GLOBALS['_photo_masknumber']);
	}

	$h='';

	// Maskenkopf
	if($top)
	{
		$h .= '<table id="mt'.$mnr.'_rahmen" border="0" cellpadding="0" cellspacing="0" style="position:relative;float:none;">'.$GLOBALS['nl'];
		$h .= '	<tr>'.$GLOBALS['nl'];
		$h .= '		<td align="left">'.$GLOBALS['nl'];
		$h .= '			<div class="maskc" style="position:relative;width:'.$width.';white-space:normal;">'.$GLOBALS['nl'];

		$h .= '				<div id="mt0_kopf" class="maskh" style="position:relative;">'.$GLOBALS['nl'];
		$h .= '					<span class="maskht">'.$top.'</span>'.$GLOBALS['nl'];
		if($mnr)
		{
			$mha=maskHelpFile($GLOBALS['fxpglobals']['tr'],$mnr,$GLOBALS['fxpglobals']['lang']);
			$h .= '					<img id="fxphoto_'.$mnr.'" src="'.$GLOBALS['gfxpath'].'im_photo'.$mha['ex'].'.png" tooltip="Take Mask Screenshot: '.$mha['id'].'" style="position:absolute;top:0;right:-36px;cursor:pointer;display:none;">'.$GLOBALS['nl'];
			$h .= '					<a href="'.$mha['mfilepath'].'?'.$GLOBALS['datetime'].'" target="fxpmaskscreenshots" tooltip="Open Mask Screenshot: '.$mha['mfilepath'].'"><img id="fxphoto_'.$mnr.'_disp" src="'.$GLOBALS['gfxpath'].'im_enl.png" style="position:absolute;top:0;right:-62px;cursor:pointer;display:none;"></a>'.$GLOBALS['nl'];
		}
		$h .= '				</div>'.$GLOBALS['nl'];

		$h .= '				<div class="maskm" style="white-space:normal;">'.$GLOBALS['nl'];
		$h .= '					<table border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
		$h .= '						<tr>'.$GLOBALS['nl'];
		$h .= '							<td>'.$GLOBALS['nl'];
	}

	// Maskeninhalt
	if($inhalt)
		$h .= $inhalt.$GLOBALS['nl'];

	// Maskenfuss
	if($bottom)
	{
		$h .= '</td>'.$GLOBALS['nl'];
		$h .= '						</tr>'.$GLOBALS['nl'];
		$h .= '					</table>'.$GLOBALS['nl'];
		$h .= '				</div>'.$GLOBALS['nl'];
		$h .= '			</div>'.$GLOBALS['nl'];
		$h .= '		</td>'.$GLOBALS['nl'];
		$h .= '	</tr>'.$GLOBALS['nl'];
		$h .= '</table>'.$GLOBALS['nl'];
	}

	echo($h);
}

/**
 * ???FUNCTION???
 *
 * @param string  $con      - Optional parameter (default = 'and'): ???PARAMETER???
 * @param boolean $mandant  - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $order    - Optional parameter (default = false): ???PARAMETER???
 * @param string  $feldname - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $prights  - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $all      - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function filter_person($con='and', $mandant=true, $order=false, $feldname='', $prights=true, $all=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$flt=512;
	$where='';

	// Globale Rechte auf Personen
	if((!isset($GLOBALS['ignore_personenfilter']) || !$GLOBALS['ignore_personenfilter']) && $GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fperson_inaktiv'])
	{
		// Suchname
		if(!$GLOBALS['ignore_name'] && strlen($GLOBALS['fxpglobals']['filter']['pname']))
		{
			$where .= " AND (".db_like('p.pname', '%'.$GLOBALS['fxpglobals']['filter']['pname'].'%');
			$where .= " OR ".db_like('p.vorname', '%'.$GLOBALS['fxpglobals']['filter']['pname'].'%');
			$where .= " OR ".db_like('p.name_zusatz', '%'.$GLOBALS['fxpglobals']['filter']['pname'].'%');
			$where .= ")";
		}
		// Projekt-Team
		if($all && !$GLOBALS['ignore_projektteam'] && $GLOBALS['fxpglobals']['filter']['projektteam_id'])
		{
			$sql="SELECT personen_id FROM projektteam WHERE wertetabid=".$GLOBALS['fxpglobals']['filter']['projektteam_id']." AND mandanten_id=";
			if(gettype($mandant) != 'boolean')
				$sql .= $mandant;
			else
				$sql .= $GLOBALS['fxpglobals']['client'];
			$fpids=db_values($sql, '*');
			if(is_array($fpids))
			{
				$ids=array_to_csv($fpids, 'personen_id');
				$where .= " AND p.personen_id IN (".$ids.")";
			}
		}
		// Vorgesetzter (für Int. Mitarbeiter)
		$mi_elt='';
		if($all && !$GLOBALS['ignore_elter'] && $GLOBALS['fxpglobals']['filter']['vorgesetzter'])
			$mi_elt=$GLOBALS['fxpglobals']['filter']['vorgesetzter'];
		// Abteilung (für Int. und Ext. Mitarbeiter)
		if($all && !$GLOBALS['ignore_abt_id'] && $GLOBALS['fxpglobals']['filter']['abt_id'])
			$where .= " AND (p.personentyp NOT IN (275,840) OR p.abt_id=".$GLOBALS['fxpglobals']['filter']['abt_id'].")";
		// Lieferant (für Ext. Mitarbeiter)
		$me_elt='';
		if($all && !$GLOBALS['ignore_person_alias'] && is_array($GLOBALS['fxpglobals']['filter']['person_alias']) && ($GLOBALS['fxpglobals']['filter']['person_alias'][0] != 0))
		{
			$ids=implode(',', $GLOBALS['fxpglobals']['filter']['person_alias']);
			if(strlen($ids))
			{
				if(strlen($me_elt))
					$me_elt .= ",";
				$me_elt .= $ids;
			}
		}
		// Personentyp
		if($all && !$GLOBALS['ignore_personentyp'] && is_array($GLOBALS['fxpglobals']['filter']))
		{
			$tmp="";
			$cnt=0;
			foreach($GLOBALS['fxpglobals']['filter'] as $feld => $wert)
			{
				if((substr($feld, 0, 12) == 'personentyp_') && $wert)
				{
					$tmp .= " OR p.personentyp=".(int)substr($feld, 12);
					$cnt++;
				}
			}
			if($cnt && ($cnt < 10))	// Bei 10 Einträgen wurde alles angehakt => Einschränkung nicht nötig
				$where .= " AND (".substr($tmp, 4).")";
		}
		// Fähigkeit
		if($all && !$GLOBALS['ignore_mafaehigkeit_id'] && is_array($GLOBALS['fxpglobals']['filter']['mafaehigkeit_id']) && ($GLOBALS['fxpglobals']['filter']['mafaehigkeit_id'][0] != 0))
		{
			$ida='';
			$idf='';
			foreach($GLOBALS['fxpglobals']['filter']['mafaehigkeit_id'] as $f)
			{
				if(substr($f, -1) == 'f')
					$idf .= ','.(int)$f;
				else
					$ida .= ','.(int)$f;
			}
			$zus="";
			if($ida)
				$zus .= " OR maart_id IN (".substr($ida, 1).")";
			if($idf)
				$zus .= " OR mafaehigkeit_id IN (".substr($idf, 1).")";
			$sql="SELECT personen_id FROM ma_art_faehigkeiten WHERE ";
			if($mandant === true)
				$sql .= "mandanten_id=".$GLOBALS['fxpglobals']['client']." AND ";
			else if(strlen($mandant))
				$sql .= "mandanten_id=".$mandant." AND ";
			$sql .= "(".substr($zus, 4).")";
			$fpids=db_values($sql, '*');
			if(is_array($fpids))
			{
				$ids=array_to_csv($fpids, 'personen_id');
				$where .= " AND p.personen_id IN (".$ids.")";
			}
		}
		// inaktiv
		if(!$GLOBALS['ignore_inaktiv'] && !$GLOBALS['fxpglobals']['filter']['inaktiv'])
			$where .= " AND (p.inaktiv=0 OR p.inaktiv IS NULL)";

		// ...Elter
		if(strlen($mi_elt) && strlen($me_elt))
			$where .= " AND (p.personentyp NOT IN (275,840) OR (p.personentyp=275 AND p.elter=".$mi_elt.") OR (p.personentyp=840 AND p.elter IN (".$me_elt.")))";
		else if(strlen($mi_elt))
			$where .= " AND (p.personentyp<>275 OR (p.personentyp=275 AND p.elter=".$mi_elt."))";
		else if(strlen($me_elt))
			$where .= " AND (p.personentyp<>840 OR (p.personentyp=840 AND p.elter IN (".$me_elt.")))";

		if(strlen($where))
			$flt |= 2052;
		else
			$flt |= 1;
	}
	else
		$flt |= 1026;

	// Recht auf alle Personen
	if($prights)
	{
		$arp=hasRightsForAllPersons();
		if(!$arp)
		{
			$pers_rights_csv=get_persons();
			if(strlen($pers_rights_csv))
				$where .= " AND p.personen_id IN (".$pers_rights_csv.")";
		}
	}

	if($feldname)
	{
		$GLOBALS[$feldname.'_flt']=$flt;
		$GLOBALS['mskfilter'] |= $flt;
	}
//echo('<b>Filter Personen:</b> '.$flt.' -- GLOBAL: '.$GLOBALS['mskfilter'].'<br />');

	// Ignore-Parameter zurücksetzen
	if(!$GLOBALS['keep_ignore'])
	{
		if(isset($GLOBALS['ignore_name']))
			unset($GLOBALS['ignore_name']);
		if(isset($GLOBALS['ignore_projektteam']))
			unset($GLOBALS['ignore_projektteam']);
		if(isset($GLOBALS['ignore_elter']))
			unset($GLOBALS['ignore_elter']);
		if(isset($GLOBALS['ignore_abt_id']))
			unset($GLOBALS['ignore_abt_id']);
		if(isset($GLOBALS['ignore_person_alias']))
			unset($GLOBALS['ignore_person_alias']);
		if(isset($GLOBALS['ignore_personentyp']))
			unset($GLOBALS['ignore_personentyp']);
		if(isset($GLOBALS['ignore_mafaehigkeit_id']))
			unset($GLOBALS['ignore_mafaehigkeit_id']);
		if(isset($GLOBALS['ignore_inaktiv']))
			unset($GLOBALS['ignore_inaktiv']);
		if(isset($GLOBALS['ignore_personenrechte']))
			unset($GLOBALS['ignore_personenrechte']);
	}

	// Mandanten-ID
	if($mandant != false)
	{
		if($mandant === true)
			$where .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		else if(strlen($mandant))
			$where .= " AND p.mandanten_id=".$mandant;
	}

	// SQL zusammensetzen
	if(strlen($where))
		$sql=" ((p.personen_id=".$GLOBALS['fxpglobals']['person']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'].") OR (".substr($where, 5)."))".$order_sql;
	else
		$sql=" p.personen_id>0".$order_sql;

	// Order
	if($order != false)
	{
		if($order === true)
			$sql .= " ORDER BY p.pname, p.vorname";
		else if(strlen($order))
			$sql .= " ORDER BY ".$order;
	}

	if(strtolower($con) == 'from')
		return " FROM personen p WHERE".$sql;
	else if(strtolower($con) == 'and')
		return " AND".$sql;
	return $sql;
}

/**
 * ???FUNCTION???
 *
 * @param various $von      - Mandatory parameter:                  ???PARAMETER???
 * @param various $bis      - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $mandant  - Optional parameter (default = true):  ???PARAMETER???
 * @param string  $feldname - Optional parameter (default = ''):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function filter_zeit($von, $bis, $mandant=true, $feldname='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$flt=512;
	$where='';

	if((!isset($GLOBALS['ignore_zeitfilter']) || !$GLOBALS['ignore_zeitfilter']) && $GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fzeit_inaktiv'])
	{
		$wpos=strpos($von, '.');
		if($wpos !== false)
			$wpos=substr($von, 0, $wpos+1);
		else
			$wpos='';

		// Startdatum - Enddatum
		if(!$GLOBALS['ignore_von'] && !$GLOBALS['ignore_bis'])
		{
			if(strlen($GLOBALS['fxpglobals']['filter']['ist_beg_dtm']) && strlen($GLOBALS['fxpglobals']['filter']['ist_end_dtm']))
				$where .= " AND ($von<='".substr($GLOBALS['fxpglobals']['filter']['ist_end_dtm'], 0, 8)."240000' AND $bis>='".substr($GLOBALS['fxpglobals']['filter']['ist_beg_dtm'], 0, 8)."000000')";
			else if(strlen($GLOBALS['fxpglobals']['filter']['ist_beg_dtm']))
				$where .= " AND $bis>='".substr($GLOBALS['fxpglobals']['filter']['ist_beg_dtm'], 0, 8)."000000'";
			else if(strlen($GLOBALS['fxpglobals']['filter']['ist_end_dtm']))
				$where .= " AND $von<='".substr($GLOBALS['fxpglobals']['filter']['ist_beg_dtm'], 0, 8)."240000'";
		}

		if(strlen($where))
			$flt |= 2080;
		else
			$flt |= 8;
	}
	else
		$flt |= 1040;

	if($feldname)
	{
		$GLOBALS[$feldname.'_flt']=$flt;
		$GLOBALS['mskfilter'] |= $flt;
	}
//echo('<b>Filter Zeit:</b> '.$flt.' -- GLOBAL: '.$GLOBALS['mskfilter'].'<br />');

	// Ignore-Parameter zurücksetzen
	if(!$GLOBALS['keep_ignore'])
	{
		if(isset($GLOBALS['ignore_von']))
			unset($GLOBALS['ignore_von']);
		if(isset($GLOBALS['ignore_bis']))
			unset($GLOBALS['ignore_bis']);
	}

	// Mandanten-ID
	if($mandant != false)
	{
		if($mandant === true)
			$where .= " AND ".$wpos."mandanten_id=".$GLOBALS['fxpglobals']['client'];
		else if(strlen($mandant))
			$where .= " AND ".$wpos."mandanten_id=".$mandant;
	}

	return $where;
}

/**
 * ???FUNCTION???
 *
 * @param string  $con      - Optional parameter (default = 'and'): ???PARAMETER???
 * @param boolean $mandant  - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $order    - Optional parameter (default = true):  ???PARAMETER???
 * @param string  $feldname - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $prights  - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function filter_projekt($con='and', $mandant=true, $order=true, $feldname='', $prights=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line']; echo('<b>filter_projekt</b> '.$dbbtf.' ('.$dbbtl.')<hr />');

	$flt=512;
	$where='';
	$where2='';

	if((!isset($GLOBALS['ignore_projektfilter']) || !$GLOBALS['ignore_projektfilter']) && $GLOBALS['fxpglobals']['setfilter'] && !$GLOBALS['fxpglobals']['filter']['fprojekt_inaktiv'])
	{
		// Suchtext
		if(!$GLOBALS['ignore_projektname'] && strlen($GLOBALS['fxpglobals']['filter']['projektname']))
		{
			$pna=explode(' ', $GLOBALS['fxpglobals']['filter']['projektname']);
			$where .= " AND (";
			foreach($pna as $pnn => $pnl)
			{
				if($pnn)
					$where .= " OR ";
				$where .= "(".db_like('p.name_projekt', "%".trim($pnl)."%").")";
			}
			$where .= ")";
		}
		// Projekt von - Projekt bis
		if(!$GLOBALS['ignore_soll_beginn'] && !$GLOBALS['ignore_soll_ende'])
		{
			if(strlen($GLOBALS['fxpglobals']['filter']['proj_menu_von']) && strlen($GLOBALS['fxpglobals']['filter']['proj_menu_bis']))
				$where .= " AND (z.soll_beginn<='".substr($GLOBALS['fxpglobals']['filter']['proj_menu_bis'], 0, 8)."240000' AND z.soll_ende>='".substr($GLOBALS['fxpglobals']['filter']['proj_menu_von'], 0, 8)."000000')";
			else if(strlen($GLOBALS['fxpglobals']['filter']['proj_menu_von']))
				$where .= " AND z.soll_ende>='".substr($GLOBALS['fxpglobals']['filter']['proj_menu_von'], 0, 8)."000000'";
			else if(strlen($GLOBALS['fxpglobals']['filter']['proj_menu_bis']))
				$where .= " AND z.soll_beginn<='".substr($GLOBALS['fxpglobals']['filter']['proj_menu_bis'], 0, 8)."240000'";
		}
		// Mutterprojekt
		if(!$GLOBALS['ignore_mutterprojekt_id'] && is_array($GLOBALS['fxpglobals']['filter']['mutterprojekt_id']) && ($GLOBALS['fxpglobals']['filter']['mutterprojekt_id'][0] != 0))
		{
			$ids=implode(',', $GLOBALS['fxpglobals']['filter']['mutterprojekt_id']);
			$where .= " AND p.mutterprojekt_id IN (".$ids.")";
		}
		// Auftraggeber
		if(!$GLOBALS['ignore_kunde'] && is_array($GLOBALS['fxpglobals']['filter']['kunde']) && ($GLOBALS['fxpglobals']['filter']['kunde'][0] != 0))
		{
			$ids=implode(',', $GLOBALS['fxpglobals']['filter']['kunde']);
			$where .= " AND p.kunde IN (".$ids.")";
		}
		// Projektmanager
		if(!$GLOBALS['ignore_projektmanager'] && is_array($GLOBALS['fxpglobals']['filter']['projektmanager']) && ($GLOBALS['fxpglobals']['filter']['projektmanager'][0] != 0))
		{
			$ids=implode(',', $GLOBALS['fxpglobals']['filter']['projektmanager']);
			$where .= " AND p.projektmanager IN (".$ids.")";
		}
		// Projektleiter
		if(!$GLOBALS['ignore_projektleiter'] && is_array($GLOBALS['fxpglobals']['filter']['projektleiter']) && ($GLOBALS['fxpglobals']['filter']['projektleiter'][0] != 0))
		{
			$ids=implode(',', $GLOBALS['fxpglobals']['filter']['projektleiter']);
			$where .= " AND (p.projektleiter IN (".$ids.") OR p.vertreter IN (".$ids."))";
		}
		// Projekt-Kategorie
		if(!$GLOBALS['ignore_kategorie_id'] && is_array($GLOBALS['fxpglobals']['filter']['kategorie_id']) && ($GLOBALS['fxpglobals']['filter']['kategorie_id'][0] != 0))
		{
			$ids=implode(',', $GLOBALS['fxpglobals']['filter']['kategorie_id']);
			$kids=pk_ids($ids);
			$where .= " AND p.kategorie_id IN (".$kids.")";
		}
		// Projekt-Status
		if(!$GLOBALS['ignore_projekt_status'] && is_array($GLOBALS['fxpglobals']['filter']))
		{
			$tmp='';
			$cnt=0;
			foreach($GLOBALS['fxpglobals']['filter'] as $feld => $wert)
			{
				if((substr($feld, 0, 14) == 'projektstatus_') && $wert)
				{
					$tmp .= " OR p.projekt_status=".(int)substr($feld, 14);
					$cnt++;
				}
			}
			if($cnt < 4 && $cnt)	// Bei 4 Einträgen wurde alles angehakt => Einschränkung nicht nötig
				$where .= " AND (".substr($tmp, 4).")";
		}
		// Priorität
		if(!$GLOBALS['ignore_prioritaet'] && is_array($GLOBALS['fxpglobals']['filter']))
		{
			$tmp='';
			$cnt=0;
			foreach($GLOBALS['fxpglobals']['filter'] as $feld => $wert)
			{
				if((substr($feld, 0, 11) == 'prioritaet_') && $wert)
				{
					$tmp .= " OR p.prioritaet=".(int)substr($feld, 11);
					$cnt++;
				}
			}
			if($cnt < 4 && $cnt)	// Bei 4 Einträgen wurde alles angehakt => Einschränkung nicht nötig
				$where .= " AND (".substr($tmp, 4).")";
		}
		// Aufgaben anz.
		if(!$GLOBALS['ignore_projektart'] && !$GLOBALS['fxpglobals']['filter']['aufgaben'])
			$where .= " AND p.projektart<=".FXP_PRJ_SUB;
		// Archiv ausbl.
		if(!$GLOBALS['ignore_archiv'] && $GLOBALS['fxpglobals']['filter']['projekt_archiviert'])
		{
			$asql="SELECT projekt_id FROM projekte WHERE projekt_archiviert=1 AND projektart=".FXP_PRJ_MAIN." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$achk=(int)db_value($asql);
			if($achk)
				$where .= " AND p.mutterprojekt_id NOT IN (".$asql.")";
		}
		// Nur Auftragsv.
		if(!$GLOBALS['ignore_auftrag'] && $GLOBALS['fxpglobals']['filter']['nurauftrag'])
		{
			$asql="SELECT projekt_id FROM auftrag WHERE projekt_id IS NOT NULL AND projekt_id>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$achk=(int)db_value($asql);
			if($achk)
				$where .= " AND p.mutterprojekt_id IN (".$asql.")";
			else
				$where .= " AND (p.mutterprojekt_id IS NULL OR p.mutterprojekt_id=0)";
		}

		if(strlen($where))
			$flt |= 2304;
		else
			$flt |= 64;
	}
	else
		$flt |= 1152;

	// Filter auf Projektrechte einschränken
	if($prights)
	{
		$wadd=prights_get(0,0,1);
		if(strlen($wadd))
			$where .= " AND p.projekt_id IN (".$wadd.")";
	}

	$where .= $where2;

	if($feldname)
	{
		$GLOBALS[$feldname.'_flt']=$flt;
		$GLOBALS['mskfilter'] |= $flt;
	}
//echo('<b>Filter Projekte:</b> '.$flt.' -- GLOBAL: '.$GLOBALS['mskfilter'].'<br />');

	// Ignore-Parameter zurücksetzen
	if(!$GLOBALS['keep_ignore'])
	{
		if(isset($GLOBALS['ignore_projektname']))
			unset($GLOBALS['ignore_projektname']);
		if(isset($GLOBALS['ignore_soll_beginn']))
			unset($GLOBALS['ignore_soll_beginn']);
		if(isset($GLOBALS['ignore_soll_ende']))
			unset($GLOBALS['ignore_soll_ende']);
		if(isset($GLOBALS['ignore_mutterprojekt_id']))
			unset($GLOBALS['ignore_mutterprojekt_id']);
		if(isset($GLOBALS['ignore_kunde']))
			unset($GLOBALS['ignore_kunde']);
		if(isset($GLOBALS['ignore_projektmanager']))
			unset($GLOBALS['ignore_projektmanager']);
		if(isset($GLOBALS['ignore_projektleiter']))
			unset($GLOBALS['ignore_projektleiter']);
		if(isset($GLOBALS['ignore_kategorie_id']))
			unset($GLOBALS['ignore_kategorie_id']);
		if(isset($GLOBALS['ignore_projekt_status']))
			unset($GLOBALS['ignore_projekt_status']);
		if(isset($GLOBALS['ignore_prioritaet']))
			unset($GLOBALS['ignore_prioritaet']);
		if(isset($GLOBALS['ignore_projektart']))
			unset($GLOBALS['ignore_projektart']);
		if(isset($GLOBALS['ignore_archiv']))
			unset($GLOBALS['ignore_archiv']);
		if(isset($GLOBALS['ignore_auftrag']))
			unset($GLOBALS['ignore_auftrag']);
	}

	// Mandanten-ID
	if($mandant != false)
	{
		if($mandant === true)
			$where .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		else if(strlen($mandant))
			$where .= " AND p.mandanten_id=".$mandant." AND z.mandanten_id=".$mandant;
	}

	$orderby="";
	if($order != false)
	{
		if($order === true)
		{
			// nach Projektnummern sortieren
			if(strtolower($GLOBALS['fxpglobals']['settings']['zeit_projektnr']) == 'ja')
				$orderby=" ORDER BY p.vorgangsnummer";
			else
				$orderby=" ORDER BY p.name_projekt";
		}
		else if(strlen($order))
			$orderby=" ORDER BY ".$order;
	}

	if(strtolower($con) == 'from')
	{
		if(strlen($where))
			return " FROM projekte p, zeitdaten z WHERE z.projekt_id=p.projekt_id AND z.zeitart=102 AND".substr($where, 4).$orderby;
		else
			return " FROM projekte p, zeitdaten z WHERE z.projekt_id=p.projekt_id AND z.zeitart=102";
	}
	else if(strtolower($con) == 'and')
		return $where.$orderby;
	return substr($where, 4).$orderby;
}

/**
 * ???FUNCTION???
 *
 * @param string  $align - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $fv    - Optional parameter (default = false): ???PARAMETER???
 * @param boolean $tt    - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function filter_icon($align='', $fv=false, $tt=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$flticzs='';
	if(strtolower(substr($GLOBALS['fxpglobals']['settings']['filterrahmen'], 0, 1)) == 'j')
	{
		if($GLOBALS['mskfilter'] & 2048)		// Filter aktiv
			$flticzs='_g';
		else if($GLOBALS['mskfilter'] & 1024)	// Filter inaktiv
			$flticzs='_r';
		else									// Filter normal
			$flticzs='_b';
	}

	$flticon  = '<img id="fxfltmsk" src="'.$GLOBALS['gfxpath'].'im_filter'.$flticzs.'.png"';
	if(strlen($align))
		$flticon .= ' align="'.$align.'"';
	$flticon .= ' style="margin:1px 5px;cursor:pointer;" '.filter_tooltip($fv, $tt).'>';

	return $flticon;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $fv - Optional parameter (default = false): ???PARAMETER???
 * @param boolean $tt - Optional parameter (default = true):  ???PARAMETER???
 */
function filter_tooltip($fv=false, $tt=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$filter_tt='';
	$filter_var='mskfilter';
	if($fv !== false)
		$filter_var='gmskfilter';
//echo('$gmskfilter=['.$GLOBALS['gmskfilter'].'], $mskfilter=['.$GLOBALS['mskfilter'].']<br />');

	if($GLOBALS[$filter_var])
	{
		$filter_ar=array(array(2,16,128), array(5,40,320));

		if(!is_array($GLOBALS['_filter_literals']))
		{
			$GLOBALS['_filter_literals']=explode(',', meldung(72, false));	// Personenfilter,Zeitfilter,Projektfilter
			$GLOBALS['_filter_literals'][277]=meldung(277, false);			// 277: Aktive(r) Filter
			$GLOBALS['_filter_literals'][315]=meldung(315, false);			// 315: Inaktive(r) Filter
			$GLOBALS['_filter_literals'][316]=' '.meldung(316, false);		// 316: (ohne Einschränkung)
		}
		if(!is_array($GLOBALS['_filter_fields']))
		{
			$GLOBALS['_filter_fields']=array(
				// Personenfilter
				'pname'					=> get_text(942, '15'),		// Suchtext
				'projektteam_id'		=> get_text(1586, '15'),	// Projekt-Team
				'vorgesetzter'			=> get_text(770, '15'),		// Vorgesetzter
				'mafaehigkeit_id'		=> get_text(375, '15'),		// Fähigkeit
				'abt_id'				=> get_text(871, '15'),		// Abteilung
				'person_alias'			=> meldung(2091, false),	// Lieferant
				'personentyp'			=> get_text(727, '15'),		// PersonenTyp
				'inaktiv'				=> get_text(1391, '15'),	// inaktiv

				// Zeitfilter
				'ist_beg_dtm'			=> get_text(129, '10'),		// Tat. Start
				'ist_end_dtm'			=> get_text(67, '10'),		// Tat. Ende

				// Vorgangsfilter
				'projektname'			=> get_text(942, '15'),		// Suchtext
				'proj_menu_von'			=> get_text(934, '15'),		// Projekte von
				'proj_menu_bis'			=> get_text(935, '15'),		// Projekte bis
				'mutterprojekt_id'		=> get_text(404, '15'),		// Mutterprojekt
				'kunde'					=> get_text(120, '15'),		// Auftraggeber
				'projektmanager'		=> get_text(1454, '15'),	// Projektmanager
				'projektleiter'			=> get_text(121, '15'),		// Projektleiter
				'kategorie_id'			=> get_text(1, '15'),		// Prj.-Kategorie
				'projekt_status'		=> get_text(137, '15'),		// Status
				'prioritaet'			=> get_text(47, '15'),		// Priorität
				'aufgaben'				=> get_text(1057, '15'),	// Aufgaben anz.
				'projekt_archiviert'	=> get_text(1974, '15'),	// Archiv ausbl.
				'nurauftrag'			=> get_text(1658, '15')		// Nur Auftragsv.
			);
		}

		$csp=1;
		if($fv || (strtolower(substr($GLOBALS['fxpglobals']['settings']['filterrahmen'], 0, 1)) == 'j'))
			$csp=2;

		// Aktive(r) Filter
		if($GLOBALS[$filter_var] & 365)
		{
			$fhnr=277;
			if(!strlen($filter_tt))
			{
				if($tt)
					$filter_tt .= ' tooltip="';
				$filter_tt .= '<table width=100% border=0 cellpadding=2 cellspacing=0>';
			}
			$filter_tt .= '<tr><td colspan='.$csp.' nowrap><u class=\'green bold\'>'.$GLOBALS['_filter_literals'][$fhnr].':</u></td></tr>';
			for($fnr=0; $fnr<3; $fnr++)
			{
				if($GLOBALS[$filter_var] & $filter_ar[1][$fnr])
				{
					$cls='009f6b';
					$txt='&nbsp;<font class=black>'.$GLOBALS['_filter_literals'][$fnr].'</font>&nbsp;';
					if(!($GLOBALS[$filter_var] & ($filter_ar[1][$fnr]*4)))
					{
						$cls='006b9f';
						$txt .= '<i class=grey>'.$GLOBALS['_filter_literals'][316].'</i>&nbsp;';
					}

					$filter_tt .= '<tr>';
					if($csp > 1)
						$filter_tt .= '<td width=10 valign=top><div style=\'border:1px solid #444444;background:#'.$cls.';border-radius:8px;box-shadow:2px 2px 6px #aaaaaa;\'>&nbsp;&nbsp;</div></td>';
					$filter_tt .= '<td nowrap>'.$txt;

					// Detailiert
					if($cls == '009f6b')
					{
						$filter_tt .= '<br /><ul style=margin-left:20px;>';
						switch($fnr)
						{
							case 0:	// Personenfilter
								$ptcnt=0;
								if(isset($GLOBALS['fxpglobals']['filter']) && is_array($GLOBALS['fxpglobals']['filter']) && sizeof($GLOBALS['fxpglobals']['filter']))
								{
									foreach($GLOBALS['fxpglobals']['filter'] as $feld => $wert)
									{
										if((substr($feld, 0, 12) == 'personentyp_') && $wert)
											$ptcnt++;
									}
								}
								$fda=array(
									'spname'			=> 'ignore_name',
									'iprojektteam_id'	=> 'ignore_projektteam',
									'ivorgesetzter'		=> 'ignore_elter',
									'iabt_id'			=> 'ignore_abt_id',
									'mperson_alias'		=> 'ignore_person_alias',
									'!personentyp'		=> 'ignore_personentyp',
									'!mafaehigkeit_id'	=> 'ignore_mafaehigkeit_id',
									'cinaktiv'			=> 'ignore_inaktiv'
								);
								foreach($fda as $fv => $fi)
								{
									$ft=substr($fv, 0, 1);
									$fv=substr($fv, 1);
									$fs=false;
									if(!$GLOBALS[$fi])
									{
										if($ft != '!')
										{
											if(($ft == 's') && strlen($GLOBALS['fxpglobals']['filter'][$fv]))
												$fs=true;
											else if(($ft == 'i') && $GLOBALS['fxpglobals']['filter'][$fv])
												$fs=true;
											else if(($ft == 'c') && !$GLOBALS['fxpglobals']['filter'][$fv])
												$fs=true;
											else if(($ft == 'm') && is_array($GLOBALS['fxpglobals']['filter'][$fv]) && ($GLOBALS['fxpglobals']['filter'][$fv][0] != 0))
												$fs=true;
										}
										else if(($fv == 'personentyp') && $ptcnt && ($ptcnt < 10))
											$fs=true;
										else if(($fv == 'mafaehigkeit_id') && !$GLOBALS['ignore_mafaehigkeit_id'] && is_array($GLOBALS['fxpglobals']['filter']['mafaehigkeit_id']) && ($GLOBALS['fxpglobals']['filter']['mafaehigkeit_id'][0] != 0))
											$fs=true;
									}

									if($fs)
										$filter_tt .= '<li class=\'s1 grey\'>&nbsp;<font class=grey>'.$GLOBALS['_filter_fields'][$fv].'</font>&nbsp;</li>';
								}
							break;

							case 1:	// Zeitfilter
								if(!$GLOBALS['ignore_von'] && !$GLOBALS['ignore_bis'])
								{
									if(strlen($GLOBALS['fxpglobals']['filter']['ist_beg_dtm']))
										$filter_tt .= '<li class=\'s1 grey\'>&nbsp;<font class=grey>'.$GLOBALS['_filter_fields']['ist_beg_dtm'].'</font>&nbsp;</li>';
									if(strlen($GLOBALS['fxpglobals']['filter']['ist_end_dtm']))
										$filter_tt .= '<li class=\'s1 grey\'>&nbsp;<font class=grey>'.$GLOBALS['_filter_fields']['ist_end_dtm'].'</font>&nbsp;</li>';
								}
							break;

							case 2:	// Vorgangsfilter
								$pscnt=0;
								$prcnt=0;
								if(isset($GLOBALS['fxpglobals']['filter']) && is_array($GLOBALS['fxpglobals']['filter']))
								{
									foreach($GLOBALS['fxpglobals']['filter'] as $feld => $wert)
									{
										if((substr($feld, 0, 14) == 'projektstatus_') && $wert)
											$pscnt++;
										if((substr($feld, 0, 11) == 'prioritaet_') && $wert)
											$prcnt++;
									}
								}
								$fda=array(
									'sprojektname'			=> 'ignore_projektname',
									'!proj_menu_von'		=> 'ignore_soll_beginn',
									'!proj_menu_bis'		=> 'ignore_soll_ende',
									'mmutterprojekt_id'		=> 'ignore_mutterprojekt_id',
									'mkunde'				=> 'ignore_kunde',
									'mprojektmanager'		=> 'ignore_projektmanager',
									'mprojektleiter'		=> 'ignore_projektleiter',
									'mkategorie_id'			=> 'ignore_kategorie_id',
									'!projekt_status'		=> 'ignore_projekt_status',
									'!prioritaet'			=> 'ignore_prioritaet',
									'caufgaben'				=> 'ignore_projektart',
									'iprojekt_archiviert'	=> 'ignore_archiv',
									'inurauftrag'			=> 'ignore_auftrag'
								);
								foreach($fda as $fv => $fi)
								{
									$ft=substr($fv, 0, 1);
									$fv=substr($fv, 1);
									$fs=false;
									if(!$GLOBALS[$fi])
									{
										if($ft != '!')
										{
											if(($ft == 's') && strlen($GLOBALS['fxpglobals']['filter'][$fv]))
												$fs=true;
											else if(($ft == 'i') && $GLOBALS['fxpglobals']['filter'][$fv])
												$fs=true;
											else if(($ft == 'm') && is_array($GLOBALS['fxpglobals']['filter'][$fv]) && ($GLOBALS['fxpglobals']['filter'][$fv][0] != 0))
												$fs=true;
											else if(($ft == 'c') && !$GLOBALS['fxpglobals']['filter'][$fv])
												$fs=true;
										}
										else if(($fv == 'projekt_status') && $pscnt && ($pscnt < 4))
											$fs=true;
										else if(($fv == 'prioritaet') && $prcnt && ($prcnt < 4))
											$fs=true;
										else if(($fv == 'proj_menu_von') && !$GLOBALS['ignore_soll_beginn'] && !$GLOBALS['ignore_soll_ende'] && strlen($GLOBALS['fxpglobals']['filter'][$fv]))
											$fs=true;
										else if(($fv == 'proj_menu_bis') && !$GLOBALS['ignore_soll_beginn'] && !$GLOBALS['ignore_soll_ende'] && strlen($GLOBALS['fxpglobals']['filter'][$fv]))
											$fs=true;
									}

									if($fs)
										$filter_tt .= '<li class=\'s1 grey\'>&nbsp;<font class=grey>'.$GLOBALS['_filter_fields'][$fv].'</font>&nbsp;</li>';
								}
							break;
						}
						$filter_tt .= '</ul>';
					}

					$filter_tt .= '</td></tr>';
				}
			}
		}

		// Inaktive(r) Filter
		if(($fv !== true) && ($GLOBALS[$filter_var] & 146))
		{
			$fhnr=315;
			if(!strlen($filter_tt))
			{
				if($tt)
					$filter_tt .= ' tooltip="';
				$filter_tt .= '<table width=100% border=0 cellpadding=2 cellspacing=0>';
			}
			else
				$filter_tt .= '<tr><td colspan='.$csp.'></td></tr>';
			$filter_tt .= '<tr><td colspan='.$csp.' nowrap><u class=\'red bold\'>'.$GLOBALS['_filter_literals'][$fhnr].':</u></td></tr>';
			for($fnr=0; $fnr<3; $fnr++)
			{
				if($GLOBALS[$filter_var] & $filter_ar[0][$fnr])
				{
					$cls='e1001a';
					$txt='&nbsp;'.$GLOBALS['_filter_literals'][$fnr].'&nbsp;';

					$filter_tt .= '<tr>';
					if($csp > 1)
						$filter_tt .= '<td width=10 valign=top><div style=\'border:1px solid #444444;background:#'.$cls.';border-radius:8px;box-shadow:2px 2px 6px #aaaaaa;\'>&nbsp;&nbsp;</div></td>';
					$filter_tt .= '<td nowrap>'.$txt.'</td></tr>';
				}
			}
		}

		if(strlen($filter_tt))
		{
			$filter_tt .= '</table>';
			if($tt)
				$filter_tt .= '"';
		}
	}

	return($filter_tt);
}

/**
 * ???FUNCTION???
 *
 * @param various $id   - Mandatory parameter:                ???PARAMETER???
 * @param string  $user - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getFilterID($id, $user='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($user))
		$user=$GLOBALS['fxpglobals']['user'];

	if($id == 223)
		$fid='0';
	else
		$fid=$user.'_'.abs($id);

	return $fid;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $set    - Optional parameter (default = false): ???PARAMETER???
 * @param string  $user   - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $client - Optional parameter (default = ''):    ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getFilters($set=false, $user='', $client='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($user))
		$user=$GLOBALS['fxpglobals']['user'];
	if(!strlen($client))
		$client=$GLOBALS['fxpglobals']['client'];

	$fa=array(
		'afilter'	=> $GLOBALS['fxpglobals']['afilter'],
		'user'		=> 0,
		'id'		=> 0,
		'mode'		=> 0,
		'name'		=> '[Standard]',
		'desc'		=> '',
		'setting'	=> '',
		'selects'	=> array()
	);

	$uia=explode('_',$fa['afilter']);
//fxDebug($uia,'$uia: '.$fa['afilter'], 0);

	if(sizeof($uia) > 1)
	{
		$fa['user']=(int)$uia[0];
		$fa['id']=(int)abs($uia[1]) * -1;
		if($fa['user'] == 1)
			$fa['mode']=-1;
		else
			$fa['mode']=1;
	}

	// Get all stored filters
	$sql  = "SELECT benutzer_id, trans_id, beschreibung FROM teinstellung";
	$sql .= " WHERE ((benutzer_id=".$user." AND (trans_id=223 OR trans_id<0)) OR (benutzer_id=1 AND trans_id<0)) AND mandanten_id=".$client;
	$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql, 0);
	if(fxIsArray($tmp))
	{
		$nra=array("\r\n"=>"<br />\r\n", "\r"=>"<br />\r\n", "\n"=>"<br />\r\n");
		foreach($tmp as $t)
		{
			// Name
			$fn='[Standard]';
			$np=strpos($t['beschreibung'],"'fname'=>'");
			if($np)
			{
				$ep=strpos($t['beschreibung'],"','fdesc",$np+10);
				$fn=substr($t['beschreibung'],$np+10,$ep-$np-10);
			}
			// Description
			$fd='';
			$np=strpos($t['beschreibung'],"'fdesc'=>'");
			if($np)
			{
				$ep=strpos($t['beschreibung'],"','fpers",$np+10);
				$fd=substr($t['beschreibung'],$np+10,$ep-$np-10);
			}

			if(($t['benutzer_id'] == $fa['user']) && ($t['trans_id'] == $fa['id']))
			{
				$fa['name']=$fn;
				$fa['desc']=text_wrap(strtr($fd,$nra),true,80);
			}
			if((($t['trans_id'] == 223) && ($fa['id'] >= 0)) || (($t['benutzer_id'] == $fa['user']) && ($t['trans_id'] == $fa['id'])))
				$fa['setting']=$t['beschreibung'];

			$fa['selects'][$fn]=$t;
			$fa['selects'][$fn]['afilter']=getFilterID($t['trans_id'],$t['benutzer_id']);
			$fa['selects'][$fn]['name']=$fn;
			$fa['selects'][$fn]['desc']=$fd;
		}

		ksort($fa['selects'],SORT_STRING);
	}
//fxDebug($fa,'$fa', 0);

	if($set)
	{
		$fs=$fa['afilter'].$GLOBALS['_divstr'][2].$fa['name'].$GLOBALS['_divstr'][2].$fa['desc'].$GLOBALS['_divstr'][2].$GLOBALS['_divstr'][0].'[Standard]';
		if(sizeof($fa['selects']))
		{
			foreach($fa['selects'] as $fn => $fv)
			{
				if($fv['trans_id'] < 0)
				{
					$fs .= $GLOBALS['_divstr'][1].$fv['benutzer_id'].'_'.abs($fv['trans_id']).$GLOBALS['_divstr'][0];
					if($fv['benutzer_id'] == 1)
						$fs .= '<i class=grey>'.$fn.'</i>';
					else
						$fs .= $fn;
				}
			}
		}
		echo('<div id="fltset" style="display:none;">'.$fs.'</div>'.$GLOBALS['nl']);
	}

	return $fa;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $use_post    - Optional parameter (default = true): ???PARAMETER???
 * @param string  $filter_name - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $filter_desc - Optional parameter (default = ''):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function filter_string($use_post=true, $filter_name='', $filter_desc='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$fra=array("'"=>"\\'");

	// Schalter
	$fsw=array(
		'fperson_inaktiv', 'fzeit_inaktiv', 'fprojekt_inaktiv'
	);
	// Checkboxen
	$chk=array(
		'personentyp_1372', 'personentyp_841', 'personentyp_276', 'personentyp_620', 'personentyp_2091',
		'personentyp_2092', 'personentyp_274', 'personentyp_2090', 'personentyp_840', 'personentyp_275',
		'inaktiv',

		'projektstatus_297', 'projektstatus_300', 'projektstatus_299', 'projektstatus_301',
		'prioritaet_445', 'prioritaet_446', 'prioritaet_447', 'prioritaet_1283',
		'Aufgaben', 'projekt_archiviert', 'nurauftrag'
	);
	// Text-Eingabefelder
	$txt=array(
		'Pname', 'Ist_Beg_Dtm', 'Ist_End_Dtm', 'ProjektName', 'proj_menu_von', 'proj_menu_bis'
	);
	// Einfach-Auswahlfelder
	$sel=array(
		'projektteam_id', 'Vorgesetzter', 'Abt_ID'
	);
	// Multi-Auswahlfelder
	$msel=array(
		'Kunde', 'MaFaehigkeit_ID', 'Person_alias', 'Mutterprojekt_ID', 'Kategorie_ID', 'Projektmanager', 'Projektleiter'
	);

	// Settings-String zusammenbauen
	$str="\$GLOBALS['fxpglobals']['filter']=array(";
	// - Name und Beschreibung
	$filter_name=trim($filter_name);
	if(strlen($filter_name))
	{
		$filter_desc=trim($filter_desc);
		$str .= "'fname'=>'".strtr($filter_name,$fra)."',";
		$str .= "'fdesc'=>'".strtr($filter_desc,$fra)."',";
	}
	// - Schalter
	foreach($fsw as $feld)
	{
		$lfeld=strtolower($feld);
		$str .= "'".$lfeld."'=>";
		if(($use_post && !$_POST[$feld]) || (!$use_post && $GLOBALS['fxpglobals']['filter'][$lfeld]))
			$str .= "1";
		else
			$str .= "''";
		$str .= ",";
	}
	// - Text-Eingabefelder
	foreach($txt as $feld)
	{
		$lfeld=strtolower($feld);
		$str .= "'".$lfeld."'=>";
		if($use_post && strlen(trim($_POST[$feld])))
			$str .= $GLOBALS['dbwerte'][$lfeld];
		else if(!$use_post && strlen($GLOBALS['fxpglobals']['filter'][$lfeld]))
			$str .= "'".strtr($GLOBALS['fxpglobals']['filter'][$lfeld],$fra)."'";
		else
			$str .= "''";
		$str .= ",";
	}
	// - Einfach-Auswahlfelder
	foreach($sel as $feld)
	{
		$lfeld=strtolower($feld);
		$str .= "'".$lfeld."'=>";
		if($use_post && $_POST[$feld])
			$str .= $GLOBALS['dbwerte'][$lfeld];
		else if(!$use_post && $GLOBALS['fxpglobals']['filter'][$lfeld])
			$str .= $GLOBALS['fxpglobals']['filter'][$lfeld];
		else
			$str .= "0";
		$str .= ",";
	}
	// - Multi-Auswahlfelder
	foreach($msel as $feld)
	{
		$rval=array();
		$lfeld=strtolower($feld);
		$str .= "'".$lfeld."'=>array(";
		if($use_post)
		{
			if(!is_array($_POST[$feld]))
				$farr=explode('|', $_POST[$feld]);
			else
				$farr=$_POST[$feld];
		}
		else
			$farr=$GLOBALS['fxpglobals']['filter'][$lfeld];
		if(is_array($farr))
		{
			$values='';
			foreach($farr as $fn => $fv)
			{
				if($rval[$fv])
					continue;

				$rval[$fv]=true;

				if(($feld == 'MaFaehigkeit_ID') || ($feld == 'MaArt_ID'))
					$values .= ",'".$fv."'";
				else
					$values .= ",".$fv;
			}
		}
		else
			$values=",0";
		$str .= substr($values, 1)."),";
	}
	// - Checkboxen
	foreach($chk as $feld)
	{
		$lfeld=strtolower($feld);
		$str .= "'".$lfeld."'=>";
		if(($use_post && $_POST[$feld]) || (!$use_post && $GLOBALS['fxpglobals']['filter'][$lfeld]))
			$str .= "1";
		else
			$str .= "''";
		$str .= ",";
	}
	$str=substr($str, 0, -1).");";
//echo('<hr /><b>FILTER:</b><hr />'.fxHtmlEncode($str).'<hr />');
	return $str;
}

/**
 * ???FUNCTION???
 *
 * @param various $persid - Mandatory parameter: ???PARAMETER???
 * @param various $von    - Mandatory parameter: ???PARAMETER???
 * @param various $bis    - Mandatory parameter: ???PARAMETER???
 * @param various &$pls   - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_aufg_mail($persid, $von, $bis, &$pls)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Aktive zugeordnete Aufgaben mit Aufwand>0 ermitteln, in denen der Antragsteller fehlen würde
	$aufg_mail='';
	$pls=array();

	$sql  = "SELECT p.projekt_id, p.vorgangsnummer, p.name_projekt, p.projektleiter, p.aufwand_soll, w.tabwert, m.soll_beginn, m.soll_ende, m.aufwand_std";
	$sql .= " FROM projekte p, maplanung m, wertetab w";
	$sql .= " WHERE m.projekt_id=p.projekt_id AND m.personen_id=".(int)$persid." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND m.auslastung>0 AND (m.soll_beginn<='".$bis."' AND m.soll_ende>='".$von."')";
	$sql .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projekt_status=".FXP_PS_ACTIVE;
	$sql .= " AND w.wertetabid=p.prioritaet AND w.id_sprache=".$GLOBALS['fxpglobals']['lang']." AND w.mandanten_id=0";
	$sql .= " ORDER BY p.vorgangsnummer";
	$aufgaben=db_values($sql, '*');
//fxDebug($aufgaben, $sql);
	if(fxIsArray($aufgaben))
	{
		$hunit=' '.meldung(484);	// Std.
		$dunit=' '.meldung(917);	// Tag(e)

		// Ist-Aufwand ermitteln
		$aea=array();
		$t='';
		foreach($aufgaben as $a)
		{
			if(strlen($t))
				$t .= ", ";
			$t .= $a['projekt_id'];
		}
//echo('$t='.$t.'<hr />');
		$sql  = "SELECT projekt_id, SUM(zeitaufwand) AS ist FROM zeitdaten";
		$sql .= " WHERE personen_id=".(int)$persid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND projekt_id IN (".$t.")";
		$sql .= " GROUP BY projekt_id";
		$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $aed)
				$aea[(int)$aed['projekt_id']]=(int)$aed['ist'];
		}
//fxDebug($aea,$sql);

		// Literale: 313 (Projekt-Nr.), 814 (Name Projekt), 121 (Projektleiter), 47 (Priorität), 1325 (Zeitraum), 1297 (Resttage), 297 (Aufwand Soll), 298 (Aufwand Ist), 1298 (Restaufwand)
		$sql="SELECT id_referenz, literal_15 FROM texte WHERE id_ref_art=3 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND id_referenz IN (313,814,121,47,1325,1297,297,298,1298)";
		$tmp=db_values($sql, '*');
		$literale=array();
		if(fxIsArray($tmp))
		{
			foreach($tmp as $lit)
				$literale[$lit['id_referenz']]=$lit['literal_15'];
		}
//fxDebug($literale,'$literale');

		$aufg_mail='<br /><br />'.$nl;
		$aufg_mail .= '<table width="80%" border="0" cellpadding="2" cellspacing="1" bgcolor="#cccccc" style="border-radius:4px;">';
		$aufg_mail .= '	<tr>';
		$aufg_mail .= '		<td width="100%" bgcolor="#006b9f" style="border-top-left-radius:4px;border-top-right-radius:4px;border-bottom:1px solid #222222;"><b style="color:#ffffff;font-size:14pt;">'.meldung(1475,false).'</b>&nbsp;&nbsp;<b style="color:#ffffff;">'.get_entry($von,'datum').' - '.get_entry($bis,'datum').'</b></td>';	// Aufgaben
		$aufg_mail .= '	</tr>';
		$aufg_mail .= '	<tr>';
		$aufg_mail .= '		<td>';
		$aufg_mail .= '			<table width="100%" border="0" cellpadding="2" cellspacing="1" bgcolor="#bbbbbb">';

		// Überschriten
		$aufg_mail .= '				<tr>';
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa"><b>'.$literale[313].'</b></td>';								// Projekt-Nr.
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa"><b>'.$literale[814].'</b></td>';								// Name Projekt
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa"><b>'.$literale[121].'</b></td>';								// Projektleiter
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa" align="center"><b>'.$literale[47].'</b></td>';					// Priorität
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa" align="center"><b>'.$literale[1325].'</b></td>';				// Zeitraum
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa" align="center"><b>'.$literale[1297].'</b></td>';				// Resttage
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa" align="center" colspan="2"><b>'.$literale[297].'</b></td>';	// Aufwand-Soll
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa" align="center"><b>%</b></td>';									// %
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa" align="right"><b>'.$literale[298].'</b></td>';					// Aufwand-Ist
		$aufg_mail .= '					<td nowrap bgcolor="#aaaaaa" align="right"><b>'.$literale[1298].'</b></td>';				// Restaufwand
		$aufg_mail .= '				</tr>';

		$ca=array(array('#dddddd','#f8f8f8'), array('#d8d8d8','#f1f1f1'));
		foreach($aufgaben as $an => $ag)
		{
			$dc=$ca[$an%2][0];
			$lc=$ca[$an%2][1];

			// PL
			if(!isset($pls[$ag['projektleiter']]))
			{
				$pinf=db_values("SELECT p.pname, p.vorname, b.e_mail_adresse FROM personen p, benutzer b WHERE b.personen_id=p.personen_id AND p.personen_id=".$ag['projektleiter']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client']);
				if(is_array($pinf) && strlen(trim($pinf['vorname'])))
					$pinf['name']=trim($pinf['vorname']).' '.$pinf['pname'];
				else if(is_array($pinf))
					$pinf['name']=$pinf['pname'];
				else
					$pinf['name']='??? '.$ag['projektleiter'];
				$pls[$ag['projektleiter']]=$pinf;
			}

			// Resttage
			$dd=fxp_date_diff(substr($GLOBALS['datetime'],0,8), substr($ag['soll_ende'],0,8));
			$rd=$dd;
			if($dd <= 0)
				$rd='<font color="#e1001a">'.$dd.'</font>';

			// Aufwand Soll
			$pe=0;
			if($ag['aufwand_std'] <= 0.00)
				$ppe='<font color="#888888">'.get_entry(0,'dezimal').'</font>';
			else
			{
				$pe=$ag['aufwand_std'];
				$ppe=get_entry($pe/3600,'dezimal');
			}
			if($ag['aufwand_soll'] <= 0.00)
				$ple='<font color="#888888">'.get_entry(0,'dezimal').'</font>';
			else
				$ple=get_entry($ag['aufwand_soll']/3600,'dezimal');

			// %
			$pp=0.00;
			if($ag['aufwand_soll'] > 0)
				$pp=$ag['aufwand_std']/$ag['aufwand_soll'];
			$pp=get_entry($pp*100,'dezimal');

			// Aufwand Ist
			$ae=0;
			if(isset($aea[$ag['projekt_id']]))
			{
				$ae=$aea[$ag['projekt_id']];
				$ape=get_entry($ae/3600,'dezimal');
			}
			else
				$ape='<font color="#888888">'.get_entry(0,'dezimal').'</font>';

			// Restaufwand
			$e=$pe-$ae;
			$re=get_entry($e/3600,'dezimal');
			if(($e < 0) || ($dd <= 0))
				$re='<font color="#e1001a">'.$re.'</font>';
			else
				$re='<font color="#009f6b">'.$re.'</font>';

			$aufg_mail .= '				<tr>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$dc.'"><b>'.no_null($ag['vorgangsnummer']).'</b></td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'"><b>'.$ag['name_projekt'].'</b></td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'">'.$pls[$ag['projektleiter']]['name'].'</td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'" align="center">'.$ag['tabwert'].'</td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'" align="center">'.get_entry($ag['soll_beginn'], 'datum').' - '.get_entry($ag['soll_ende'], 'datum').'</td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'" align="right">'.$rd.$dunit.'</td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'" align="right">'.$ppe.$hunit.'</td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'" align="right">('.$ple.$hunit.')</td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'" align="right">'.$pp.' %</td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'" align="right">'.$ape.$hunit.'</td>';
			$aufg_mail .= '					<td nowrap bgcolor="'.$lc.'" align="right">'.$re.$hunit.'</td>';
			$aufg_mail .= '				</tr>';
		}
		$aufg_mail .= '			</table>';
		$aufg_mail .= '		</td>';
		$aufg_mail .= '	</tr>';
		$aufg_mail .= '</table>'.$nl;
	}

	return $aufg_mail;
}

/**
 * ???FUNCTION???
 *
 * @param integer $pid - Optional parameter (default = 0): ???PARAMETER???
 * @param integer $bid - Optional parameter (default = 0): ???PARAMETER???
 * @param integer $mid - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_reports($pid=0, $bid=0, $mid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pid)
		$pid=$GLOBALS['fxpglobals']['person'];
	if(!$bid)
		$bid=$GLOBALS['fxpglobals']['user'];
	if(!$mid)
		$mid=$GLOBALS['fxpglobals']['client'];

	// Alle Reports des Mandanten ermitteln
	$sql="SELECT repid, aenderungs_id FROM repliste WHERE mandanten_id=".$mid;
	$ar=db_values($sql, '*');
	if(!is_array($ar))
		return false;

	// Ist die Person ein Geschäftsführer?
	$sql="SELECT ma_art_faehigkeit_id FROM ma_art_faehigkeiten WHERE personen_id=".$pid." AND maart_id=1 AND mafaehigkeit_id=2 AND mandanten_id=".$mid;
	$gf=db_value($sql);

	$ra=false;
	$abt='';
	$bgr='';
	foreach($ar as $a)
	{
		// GF hat Recht auf alle Reports
		if($gf)
			$ra[$a['repid']]=255;
		// Report-Ersteller hat Recht auf Report
		else if($a['aenderungs_id'] == $pid)
			$ra[$a['repid']]=63;
		// sonst Rechteprüfung
		else
		{
			// Abteilungen einmal ermitteln
			if($abt === '')
				$abt=get_abteilungen($pid, $mid);

			// Benutzergruppen einmal ermitteln
			if($bgr === '')
				$bgr=get_benutzergruppen($bid, $mid);

			// Rechte holen
			$sql="SELECT aktionscode, gruppeart AS art, benutzergr_id AS id FROM rechte_referenz WHERE rechte_ref=35 AND id_referenz=".$a['repid']." AND mandanten_id=".$mid;
			$rr=db_values($sql, '*');
			if(is_array($rr))
			{
				foreach($rr as $r)
				{
					if(($r['art'] == 2163) && ($r['id'] == $bid))		// Benutzer
						$ra[$a['repid']]=$r['aktionscode'];
					else if(($r['art'] == 2164) && ($bgr[$r['id']]))	// Benutzergruppe
						$ra[$a['repid']]=$r['aktionscode'];
					else if(($r['art'] == 2165) && ($abt[$r['id']]))	// Abteilung
						$ra[$a['repid']]=$r['aktionscode'];
				}
			}
		}
	}

	return $ra;
}

/**
 * ???FUNCTION???
 *
 * @param string  $sadd  - Optional parameter (default = ''):    ???PARAMETER???
 * @param boolean $cpers - Optional parameter (default = false): ???PARAMETER???
 * @param integer $mid   - Optional parameter (default = 0):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getDepStructure($sadd='', $cpers=false, $mid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sadd=trim($sadd);
	if(!$mid)
		$mid=$GLOBALS['fxpglobals']['client'];

	$dsa=array();
	$dra=array();
	$dia=array(0=>true);
	while(sizeof($dia))
	{
		$dic=array_to_csv($dia);
//fxDebug($dia,'$dia: $dic='.$dic);
		$dia=array();
		$sql="SELECT * FROM abteilung WHERE mandanten_id=".$mid." AND zu_abteilung IN (".$dic.") AND (abt_id>0";
		if(strlen($sadd))
			$sql .= " ".$sadd;
		$sql .= ") ".db_orderby('abt_ebene', 'abt_name');
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $tc => $t)
			{
				$pai=$t['abt_id'];
				$pdi=$t['zu_abteilung'];
				$ndi='';
				if(isset($dra[$pdi]))
					$ndi=$dra[$pdi].'.';
				$ndi .= substr('000'.$tc,-3);
				$dra[$pai]=$ndi;
				$dia[$pai]=true;
				$dsa[$ndi]=$t;
				$dsa[$ndi]['persons']=0;
				$dsa[$ndi]['level']=substr_count($ndi,'.');
			}
		}
	}
//fxDebug($dra,'$dra');

	$ra=false;
	if(sizeof($dsa))
	{
		// Count assigned persons
		if($cpers)
		{
			$sql="SELECT personen_id,pname,vorname,abt_id FROM personen WHERE abt_id>0 AND mandanten_id=".$mid;
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					foreach($dsa as $dk => $da)
					{
						if($da['abt_id'] == $t['abt_id'])
						{
							$d=$dk;
							while(strlen($d))
							{
								$dsa[$d]['persons']++;
								$d=substr($d,0,-4);
							}
						}
					}
				}
			}
		}

		ksort($dsa, SORT_STRING);
//fxDebug($dsa,'$dsa');

		$i=0;
		foreach($dsa as $d)
		{
			$ra[$i]=$d;
			$i++;
		}
	}

//fxDebug($ra,'$ra');
	return $ra;
}

/**
 * ???FUNCTION???
 *
 * @param integer $pid         - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $mid         - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $nur_aleiter - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_abteilungen($pid=0, $mid=0, $nur_aleiter=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$abt=false;

	if(!$pid)
		$pid=$GLOBALS['fxpglobals']['person'];
	if(!$mid)
		$mid=$GLOBALS['fxpglobals']['client'];

	// Abteilungsleiter
	$uarem=array();
	$ua='';
	do
	{
		if(!$ua)
		{
			$sql="SELECT abt_id FROM abteilung WHERE abt_hperson=".$pid." AND mandanten_id=".$mid;
			$amd=8;
		}
		else
		{
			$sql="SELECT abt_id FROM abteilung WHERE zu_abteilung IN (".$ua.") AND mandanten_id=".$mid;
			$amd=4;
			$ua='';
		}
		$aa=db_values($sql, '*');
		if(is_array($aa))
		{
			foreach($aa as $a)
			{
				$naid=(int)$a['abt_id'];
				if(!$uarem[$naid] && $naid)
				{
					if(!is_array($abt) || !$abt[$naid])
						$abt[$naid]=$amd;
					else
						$abt[$naid] |= $amd;
					$ua .= ','.$naid;
					$uarem[$naid]=true;
				}
			}
			$ua=substr($ua, 1);
		}
	} while($ua);

	// Abteilung
	if(!$nur_aleiter)
	{
		$uarem='';
		$ua='';
		do
		{
			if(!$ua)
			{
				$sql="SELECT abt_id FROM personen WHERE personen_id=".$pid." AND mandanten_id=".$mid;
				$amd=2;
			}
			else
			{
				$sql="SELECT abt_id FROM abteilung WHERE zu_abteilung IN (".$ua.") AND mandanten_id=".$mid;
				$amd=1;
				$ua='';
			}
			$aa=db_values($sql, '*');
			if(is_array($aa))
			{
				foreach($aa as $a)
				{
					$naid=(int)$a['abt_id'];
					if(!$uarem[$naid] && $naid)
					{
						if(!is_array($abt) || !$abt[$naid])
							$abt[$naid]=$amd;
						else
							$abt[$naid] |= $amd;
						$ua .= ','.$naid;
						$uarem[$naid]=true;
					}
				}
				$ua=substr($ua, 1);
			}
		} while($ua);
	}

	return $abt;
}

/**
 * ???FUNCTION???
 *
 * @param integer $bid - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $mid - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $csv - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_benutzergruppen($bid=0, $mid=0, $csv=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$bid)
		$bid=$GLOBALS['fxpglobals']['user'];
	if(!$mid)
		$mid=$GLOBALS['fxpglobals']['client'];
	if(($bid == $GLOBALS['fxpglobals']['user']) && is_array($GLOBALS['fxpglobals']['persdat']) && $GLOBALS['fxpglobals']['persdat']['profileoverride'])
	{
		if($csv)
			return $GLOBALS['fxpglobals']['persdat']['profileoverride'];
		else
			return array($GLOBALS['fxpglobals']['persdat']['profileoverride'] => true);
	}

	$bgrcsv='0';

	// Benutzerguppen
	$sql="SELECT benutzergr_id FROM benutzergruppenzo WHERE benutzer_id=".$bid." AND mandanten_id=".$mid;
	$ba=db_values($sql, '*');
	if(is_array($ba))
	{
		$bgrarr=array();
		foreach($ba as $b)
		{
			$bgrarr[$b['benutzergr_id']]=true;
			$bgrcsv .= ','.$b['benutzergr_id'];
		}
	}
	else
		$bgrarr=false;
//fxDebug($bgrarr, 'Benutzerguppen von '.$bid);

	if($csv)
		return $bgrcsv;
	return $bgrarr;
}

/**
 * ???FUNCTION???
 *
 * @param integer $abtid - Optional parameter (default = 0): ???PARAMETER???
 * @param integer $mid   - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function inferior_abteilung($abtid=0, $mid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$abtid=(int)$abtid;
	if(!$abtid)
		return 0;

	$mid=(int)$mid;
	if(!$mid)
		$mid=$GLOBALS['fxpglobals']['client'];

	$uarem= array();
	$uaret= $abtid;
	$ua= $abtid;
	do
	{
		$sql="SELECT abt_id FROM abteilung WHERE zu_abteilung IN (".$ua.") AND mandanten_id=".$mid;
		$ua='';

		$aa=db_values($sql, '*');
		if(is_array($aa))
		{
			foreach($aa as $a)
			{
				$naid=(int)$a['abt_id'];
				if(!$uarem[$naid] && $naid)
				{
					if(strlen($ua))
						$ua .= ",";
					$ua .= $naid;

					$uaret .= ",".$naid;

					$uarem[$naid]=true;
				}
			}
		}
	} while($ua);

	return $uaret;
}

/**
 * ???FUNCTION???
 *
 * @param various $arr     - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $ak      - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $onlyint - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function array_to_csv($arr, $ak=true, $onlyint=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($arr))
	{
		if(strlen($arr))
			$arr[0]=$arr;
		else
			$arr[0]=0;
	}

	$cvr=array();
	$csv='';
	foreach($arr as $k => $v)
	{
		if($ak === true)
			$cv=$k;
		else if(is_array($v) && (gettype($ak) == 'string'))
			$cv=$v[$ak];
		else
			$cv=$v;

		if($onlyint)
			$cv=(int)$cv;

		if(!isset($cvr[$cv]))
		{
			$cvr[$cv]=true;
			$csv .= ','.$cv;
		}
	}

	if(strlen($csv))
		$csv=substr($csv,1);

	return $csv;
}

/**
 * ???FUNCTION???
 *
 * @param various $csv - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $ak  - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function csv_to_array($csv, $ak=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($csv))
		return array(0=>false);

	$arr=array();
	$tar=explode(',', $csv);
	foreach($tar as $z => $v)
	{
		if(!$ak)
			$arr[$z]=(int)trim($v);
		else
			$arr[(int)trim($v)]=true;
	}

	return $arr;
}

/**
 * ???FUNCTION???
 *
 * @param various $sql - Mandatory parameter:                ???PARAMETER???
 * @param string  $ke  - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $vl  - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_array($sql, $ke='', $vl='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($sql))
		return false;

	$tar=db_values($sql, '*');
//fxDebug($tar,$sql);
	if(!fxIsArray($tar))
		return false;

	if(isset($tar[0]['mandanten_id']) && isset($tar[0]['wertetabid']))	// Nachschlagewerte gesondert behandeln, d.h. Mandantenwerte überschreiben Defaultwerte
	{
		$mar=array();
		foreach($tar as $tk => $ta)
		{
			$id=$ta['wertetabid'];
			if(!isset($mar[$id]) || $ta['mandanten_id'])
				$mar[$id]=$tk;
		}
		$arr=array();
		foreach($mar as $id => $tk)
			$arr[]=$tar[$tk];
	}
	else
		$arr=$tar;
//fxDebug($arr,'$arr');

	if(!strlen($ke) && !strlen($vl))
		return $arr;

	$rar=array();
	foreach($arr as $z => $inh)
	{
		if(!strlen($ke))
			$rar[$z]=$inh[$vl];
		else if(strlen($vl))
			$rar[$inh[$ke]]=$inh[$vl];
		else
			$rar[$inh[$ke]]=$inh;
	}
	return $rar;
}

/**
 * ???FUNCTION???
 *
 * @param various $wert    - Mandatory parameter:                 ???PARAMETER???
 * @param various $maxwert - Mandatory parameter:                 ???PARAMETER???
 * @param integer $width   - Optional parameter (default = 156):  ???PARAMETER???
 * @param integer $height  - Optional parameter (default = 9):    ???PARAMETER???
 * @param string  $col1    - Optional parameter (default = 'f'):  ???PARAMETER???
 * @param string  $col2    - Optional parameter (default = 'b'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function balken($wert, $maxwert, $width=156, $height=9, $col1='f', $col2='b')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($wert && $maxwert)
		$w1=(int)($width*$wert/$maxwert);
	else
		$w1=0;
	$w2=$width-$w1;

	$html  = '<table width="'.$width.'" height="'.$height.'" border="0" cellpadding="0" cellspacing="0">';
	$html .= '<tr>';
	if($w1)
		$html .= '<td><img src="'.$GLOBALS['gfxpath'].'bk'.$col1.'.png" border="0" width="'.$w1.'" height="'.$height.'"></td>';
	if($w2)
		$html .= '<td><img src="'.$GLOBALS['gfxpath'].'bk'.$col2.'.png" border="0" width="'.$w2.'" height="'.$height.'"></td>';
	$html .= '</tr>';
	$html .= '</table>';

	return $html;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $all - Optional parameter (default = true): ???PARAMETER???
 * @param string  $arr - Optional parameter (default = ''):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function rk_kategorie_array($all=true, $arr='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!is_array($arr) || !sizeof($arr))
		$arr=fxf_getReferenceValues(1291,'wertetabid');
	if(!is_array($arr) || !sizeof($arr))
		return array();
//fxDebug($arr,'$arr');

	if(!$all)
	{
		foreach($arr as $id => $val)
		{
			$mp=strpos($val,'^');
			if($mp !== false)
				$arr[$id]=substr($val,0,$mp);
		}
	}

	return $arr;
}

/**
 * ???FUNCTION???
 *
 * @param integer $mk - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function kategorie_array($mk=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mks=(int)$mk;
	if(!$mks || ($mks !== $mk))
	{
		$mk=strtolower(trim($mk));
		switch($mk)
		{
			case 'projektart':
				$mks=57;
			break;

			default:
				return '';
		}
	}

	$sql  = "SELECT k.kategorie_id, w.tabwert FROM kategorien k, wertetab w";
	$sql .= " WHERE w.wertetabid=k.name_kategorie AND k.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND k.masterkategorie=".$mks." AND w.id_sprache=".$GLOBALS['fxpglobals']['lang'];
	$sql .= " AND w.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND (w.archiv IS NULL OR w.archiv=0)";
	$arr=get_array($sql, 'kategorie_id', 'tabwert');
//fxDebug($arr, $sql);

	return $arr;
}

/**
 * Alle Dummy- und Linienfelder einblenden
 */
function enable_dummies()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	for($d=-1; $d<6; $d++)
	{
		if($d < 0)
			$dc='';
		else
			$dc=$d;
		if(isset($GLOBALS['Dummy'.$dc.'_nv']))
			unset($GLOBALS['Dummy'.$dc.'_nv']);
		if(isset($GLOBALS['hr'.$dc.'_nv']))
			unset($GLOBALS['hr'.$dc.'_nv']);
	}
}

/**
 * Draw a horizontal bar chart
 *
 * @param double  $percent    - Optional parameter (default = 0.0):  ???PARAMETER???
 * @param string  $tt         - Optional parameter (default = ''):   ???PARAMETER???
 * @param string  $barcol     - Optional parameter (default = ''):   ???PARAMETER???
 * @param integer $bar_bsize  - Optional parameter (default = 686):  ???PARAMETER???
 * @param integer $bar_psize  - Optional parameter (default = 70):   ???PARAMETER???
 * @param integer $bar_height - Optional parameter (default = 15):   ???PARAMETER???
 * @param integer $bar_space  - Optional parameter (default = 2):    ???PARAMETER???
 * @param string  $valclass   - Optional parameter (default = ''):   ???PARAMETER???
 *
 * @return HTML code
 */
function bar_horizontal($percent=0.0, $tt='', $barcol='', $bar_bsize=686, $bar_psize=70, $bar_height=15, $bar_space=2, $valclass='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($percent < 0.00)
	{
		$percent *= -1.00;
		$vpos='right';
	}
	else
		$vpos='left';
	$dpercent=min(max($percent, 0.00), 100.00);

 	$border='border:1px solid #cbcbcb;border-right:1px solid #a0a0a0;border-bottom:1px solid #a0a0a0;';
 	if(!strlen($barcol))
	{
		if($dpercent < 100.00)
			$barcol='green';
		else
		{
			$barcol='compl';
		 	$border='border:1px solid #c5e8c9;border-right:1px solid #1cde32;border-bottom:1px solid #1cde32;';
		}
	}

	$bar_default ='<table border="0" cellpadding="0" cellspacing="0"><tr>';
	$bar_default .= '<td width="'.$bar_bsize.'" height="'.($bar_height+$bar_space).'" valign="top" nowrap>';

	$tts='';
	if(strlen($tt))
		$tts='cursor:help;" tooltip="'.$tt;

	$bar_default .= '<div style="position:relative;float:none;'.$tts.'">';

	// Hintergrund
	$bar_default .= '<div style="position:relative;left:0;top:0;float:left;"><img src="'.$GLOBALS['gfxpath'].'bar_bg.png" width="'.$bar_bsize.'" height="'.$bar_height.'" style="'.$border.'" align="left">';

	// Farbbalken
	if($dpercent)
	{
		$rest=100.00 - $dpercent;
		$psize=$bar_bsize;
		if($rest)
			$psize=(int)($bar_bsize*$dpercent / 100.00) - 1;

		$bar_default .= '<div style="position:absolute;'.$vpos.':1px;top:1px;float:none;"><img id="bardefault" src="'.$GLOBALS['gfxpath'].'bar_'.$barcol.'.png" width="'.$psize.'" height="'.$bar_height.'"></div>';
	}

	$bar_default .= '</div></div>';

	// Prozentwert
	$tdclass='';
	if(strlen($valclass))
		$tdclass=' class="'.$valclass.'"';
	$bar_default .= '</td><td'.$tdclass.' width="'.$bar_psize.'" height="'.$bar_height.'" align="right" valign="top" nowrap>&nbsp;'.get_entry($percent, 'dezimal').'%</td></tr></table>';

	return $bar_default;
}

/**
 * ???FUNCTION???
 *
 * @param various $val       - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $posisgood - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function ampelIcon($val, $posisgood=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$im='neutral';
	$val=get_double($val);
	if($posisgood)
	{
		if($val < 0.00)
			$im='red';
		else if($val > 0.00)
			$im='green';
	}
	else if($val < 0.00)
		$im='green';
	else if($val > 0.00)
		$im='red';

	return $GLOBALS['gfxpath'].'st_'.$im.'.png';
}

/**
 * ???FUNCTION???
 */
function get_history_tables()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['fxpglobals']['historytables']))
		unset($GLOBALS['fxpglobals']['historytables']);

	$sql  = "SELECT t.id_tabellen, t.tabname";
	$sql .= " FROM tabellen t, tab_felder f";
	$sql .= " WHERE f.idintabellen=t.id_tabellen AND f.idinfelder=1729 AND t.id_tabellen>0 AND t.id_tabellen<>26 AND t.historie IS NOT NULL AND t.historie<>0";
	$sql .= " ORDER BY t.tabname";
	$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql);
	if(fxIsArray($tmp))
	{
		$dbtype=$GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['type'];
		$GLOBALS['fxpglobals']['historytables']=array();

		// Alle Feldnamen der Tabelle ermitteln (ausser transid (12) + aenderungs_id (84) + zeitstempel (19))
		foreach($tmp as $t)
		{
			$tabname=strtolower(trim($t['tabname']));

			$sql  = "SELECT f.feldname FROM tab_felder tf, felder f";
			$sql .= " WHERE tf.idintabellen=".$t['id_tabellen']." AND f.id=tf.idinfelder AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].") AND tf.idinfelder NOT IN (12,84,19)";
			$sql .= " ORDER BY tf.positionbeitabelle";
			$tfelder=db_values($sql, '*');
			if(fxIsArray($tfelder))
			{
				$tfldstr='';
				foreach($tfelder as $tfld)
					$tfldstr .= ', '.strtolower($tfld['feldname']);

				if(strlen($tfldstr))
					$GLOBALS['fxpglobals']['historytables'][$tabname]=array('id'=>(int)$t['id_tabellen'], 'fld'=>$tfldstr);
			}
		}

		if(!sizeof($GLOBALS['fxpglobals']['historytables']))
			$GLOBALS['fxpglobals']['historytables']=true;
	}
	else
		$GLOBALS['fxpglobals']['historytables']=true;
}

/**
 * müssen gleich sein mit Konstanten in JavaScript-Funktionen
 *
 * @param various $days - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getBarWidth($days)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return $days * PIXELS_PER_DAY;
}

/**
 * ???FUNCTION???
 *
 * @param various $offset - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getBarOffset($offset)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return $offset * PIXELS_PER_DAY;
}

/**
 * ???FUNCTION???
 *
 * @param various $processes - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function processArrayToJSArray($processes)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(is_array($processes))
	{
		$indent= chr(9).chr(9);
		$status= fxf_getReferenceValues('projekt_status');
		$prioritaet= fxf_getReferenceValues('prioritaet');
		$code= $indent.'var processesArray=new Array('.count($processes).');'.$GLOBALS['nl'];
		foreach($processes as $key => $process)
		{
			if(is_array($process))
			{
				$code .= $indent.'processesArray['.$key.']=new Array(';
				foreach($process as $field => $value)
				{
					if($field == 'projekt_status')
						$value=$status[$value];
					else if($field == 'prioritaet')
						$value=$prioritaet[$value];
					else if($field == 'deadline')
						$value=get_entry($value, 'datum');
					else if($field == 'vorgangsnummer')
						$value=no_null($value);
					$code .= '"'.$value.'", ';
				}
				$code=substr($code, 0, -2).');'.$GLOBALS['nl'];
			}
		}
	}
	return $code;
}

/**
 * ???FUNCTION???
 *
 * @param various $mnr  - Mandatory parameter: ???PARAMETER???
 * @param various $feld - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_auswahl_breite($mnr, $feld)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($mnr))
		$mnr=$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim($mnr));

	if(!$mnr)
		return 0;

	$feld=strtoupper(trim($feld));
	$feldnr=(int)$feld;
	if(strlen($feld) && ((string)$feld !== (string)$feldnr))
	{
		$sql="SELECT id FROM felder WHERE ".db_suche('feldname', $feld)." AND (mandanten_id IS NULL OR mandanten_id=0)";
		$feldnr=(int)db_value($sql);
	}
	if(!$feldnr)
		return 0;

	$sql="SELECT anzeigegroesse2 FROM maskenfelder WHERE nummer=".$mnr." AND eingabefeld=".$feldnr." AND anzeigeart=8";
	$azg=(int)db_value($sql);

	return $azg;
}

/**
 * entscheidet ob ein String gleich dem Beginn eines anderen String ist
 * @param string $string der enthaltende String
 * @param string $prefix der enthaltene String
 * @param boolean $caseSensitive gibt an, ob case sensitive geprüft werden soll oder nicht
 * @return boolean true, wenn $prefix mit dem Beginn von $string übereinstimmt, sonst false
 */
function stringStartsWith($string, $prefix, $caseSensitive=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$caseSensitive)
	{
		$string=strtolower($string);
		$prefix=strtolower($prefix);
	}
	return(($pos=strpos($string, $prefix)) !== false && $pos == 0);
}

/**
 * entscheidet ob ein String gleich dem Ende eines anderen String ist
 * @param string $string der enthaltende String
 * @param string $suffix der enthaltene String
 * @param boolean $caseSensitive gibt an, ob case sensitive geprüft werden soll oder nicht
 * @return boolean true, wenn $suffix mit dem Ende von $string übereinstimmt, sonst false
 */
function stringEndsWith($string, $suffix, $caseSensitive=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$caseSensitive)
	{
		$string=strtolower($string);
		$suffix=strtolower($suffix);
	}
	// wäre ohne reverseString implementierbar wenn strrpos als needle einen String schlucken könnte (erst ab PHP 5)
	return stringStartsWith(strrev($string), strrev($suffix));
}

/**
 * ???FUNCTION???
 *
 * @param various $atype - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function isAbsentApprovalAppointment($atype)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(($atype == FXP_AT_VACATIONREQUEST) || ($atype == FXP_AT_SPECIALVACATIONREQUEST) || ($atype == FXP_AT_TRAININGREQUEST) || ($atype == FXP_AT_SICKCALL) || ($atype == FXP_AT_SLIDINGDAYREQUEST) || ($atype == FXP_AT_OVERTIMEOUTPAYREQUEST))
		return true;
	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $atype - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function isAbsentAppointment($atype)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(($atype == FXP_AT_VACATION) || ($atype == FXP_AT_SPECIALVACATION) || ($atype == FXP_AT_TRAINING) || ($atype == FXP_AT_SICK) || ($atype == FXP_AT_SLIDINGDAY) || ($atype == FXP_AT_OVERTIMEOUTPAY))
		return true;
	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $termin - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $prj    - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function termin_tooltip($termin, $prj=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//fxDebug($termin, '$termin');

	// Infos holen
	$tmart=$termin['terminart'];
	if(!$GLOBALS['TerminText'][$tmart])
		$GLOBALS['TerminText'][$tmart]=meldung($tmart);
	$tmart_text=$GLOBALS['TerminText'][$tmart];
	if($tmart == FXP_AT_INVITATION)
	{
		$tmart=substr($termin['anmerkung'], 1);
		$termin['anmerkung']='';
		$tmart_text .= ': '.meldung($tmart);
	}
	$ansprechpartner='';
	if($termin['ansprechpartner'])
	{
		$Anspr_query="SELECT pname, vorname FROM personen WHERE personen_id=".$termin['ansprechpartner'];
		$Anspr=db_values($Anspr_query);
		if(is_array($Anspr))
		{
			$ansprechpartner=$Anspr['pname'];
			$ansprechvorname=trim($Anspr['vorname']);
			if(strlen($ansprechvorname))
				$ansprechpartner .= ', '.$ansprechvorname;
		}
	}

	// Zeitraum
	if(($tmart == FXP_AT_OVERTIMEOUTPAYREQUEST) || ($tmart == FXP_AT_OVERTIMEOUTPAY))
		$zeitraum=meldung(17+(int)substr($termin['startzeit'], 4, 2), false).'&nbsp;'.substr($termin['startzeit'], 0, 4);
	else if(substr($termin['startzeit'], 0, 8) != substr($termin['endzeit'], 0, 8))
		$zeitraum=get_entry($termin['startzeit'], 'datetime').'&nbsp;-&nbsp;'.get_entry($termin['endzeit'], 'datetime');
	else
	{
		$zeitraum=get_entry($termin['startzeit'], 'datum').',&nbsp;'.get_entry($termin['startzeit'], 'zeit');
		if($termin['endzeit'] != $termin['startzeit'])
			$zeitraum .= '-'.get_entry($termin['endzeit'], 'zeit');
	}

	// Stunden
	$stunden='';
	if($termin['stunden'])
	{
		$stunden=get_entry($termin['stunden'], 'dezimal').' h';
		if($tmart == FXP_AT_OVERTIMEOUTPAYREQUEST)
		{
			$first=fx_date('Ymd', fx_mktime(12, 0, 0, (int)substr($termin['startzeit'], 4, 2)+1, 1, (int)substr($termin['startzeit'], 0, 4)));
			$real_overtime=summe_zeitkonto($termin['personen_id'], $first);

			$stunden .= ' / '.get_entry($real_overtime/3600, 'dezimal').' h';
		}
	}

	// Tooltip-Fenster umwandeln
	$info="<table border=0 cellpadding=1 cellspacing=1>";
	// Titelzeile [Grafik/Name/ID]
	$gfx=$GLOBALS['TerminGrafiken'][$tmart];
	if(!$gfx)
		$gfx='c_outl';
	$info .= "<tr><td colspan=2><b class=black>";
	if(!isAbsentApprovalAppointment($tmart) && !isAbsentAppointment($tmart) && strlen($termin['name_termin']))
		$info .= str_replace(' ', '&nbsp;', $termin['name_termin']);
	else
		$info .= str_replace(' ', '&nbsp;', $GLOBALS['TerminText'][$tmart]);
	$info .= "</b>";
	$info .= "<hr class=fxhr></td></tr>";
	// Terminart
	$info .= "<tr><td valign=top nowrap><span class=lit>".$GLOBALS['text_434'].":</span>&nbsp;</td><td><span class=black>".$tmart_text."</span></td></tr>";
	// Zeitraum
	$info .= "<tr><td valign=top nowrap><span class=lit>".$GLOBALS['text_1325'].":</span>&nbsp;</td><td><span class=black>".$zeitraum."</span></td></tr>";
	// Stunden
	if(strlen($stunden))
		$info .= "<tr><td valign=top nowrap><span class=lit>".$GLOBALS['text_1877'].":</span>&nbsp;</td><td><span class=black>".$stunden."</span></td></tr>";
	// Ansprechpartner
	if($ansprechpartner)
		$info .= "<tr><td valign=top nowrap><span class=lit>".$GLOBALS['text_349'].":</span>&nbsp;</td><td><span class=black>".$ansprechpartner."</span></td></tr>";
	// Vorgang
	if(fxIsArray($prj))
		$info .= "<tr><td valign=top nowrap><span class=lit>".$GLOBALS['text_66'].":</span>&nbsp;</td><td><span class=black>".no_null($prj['vorgangsnummer'])."&nbsp;&nbsp;".$prj['name_projekt']."</span></td></tr>";
	// Treffpunkt
	if(strlen(trim($termin['treffpunkt'])) && ($termin['treffpunkt'] != "-"))
		$info .= "<tr><td valign=top nowrap><span class=lit>".$GLOBALS['text_541'].":</span>&nbsp;</td><td><span class=black>".trim($termin['treffpunkt'])."</span></td></tr>";
	// Beschreibung
	if(strlen(trim($termin['beschreibung'])))
		$info .= "<tr><td valign=top nowrap><span class=lit>".$GLOBALS['text_38'].":</span>&nbsp;</td><td><span class=black>".trim($termin['beschreibung'])."</span></td></tr>";
	// Anmerkung (Erstellt)
	$anm='';
	$txt=trim($termin['anmerkung']);
	$aa=false;
	if((substr($txt,1,1) == '-') || isAbsentApprovalAppointment($tmart))
		$aa=true;
	$aca=termin_ersteller($termin);
	if(fxIsArray($aca))
	{
//fxDebug($aca,$termin['termin_id']);
		$anm=$aca['text'];
	}
	// Anmerkung ff.
	if(strlen($txt))
	{
		if($aa)
		{
			$aap='<img src='.$GLOBALS['gfxpath'].'tl_';
			if(substr($txt,1,1) == '-')
			{
				if(substr($txt,0,1) == '1')
					$aap .= 'green';
				else
					$aap .= 'red';
				$txt=substr($txt,2);
			}
			else
				$aap .= 'yellow';
			$aap .= '.png>&nbsp;&nbsp;';
			$txt = $aap.$txt;
		}
		if(strlen($anm))
			$anm .= '<br />';
		$anm .= $txt;
	}
	if(strlen($anm))
		$info .= "<tr><td valign=top nowrap><span class=lit>".$GLOBALS['text_1370'].":</span>&nbsp;</td><td><span class=black>".$anm."</span></td></tr>";
	$info .= '</table>';

	return $info;
}

/**
 * ???FUNCTION???
 *
 * @param various $termin - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function termin_ersteller($termin)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$aca=false;
	$sql="SELECT aenderungs_id,zeitstempel FROM termine_history WHERE termin_id=".(int)$termin['termin_id']." AND aktion=2 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY autoid";
	$acd=db_values($sql);
	if(fxIsArray($acd))
	{
//fxDebug($acd,$sql);
		$tmart=$termin['terminart'];
		$txt=trim($termin['anmerkung']);
		$aa=false;
		if((substr($txt,1,1) == '-') || isAbsentApprovalAppointment($tmart))
			$aa=true;
		$pid=(int)$acd['aenderungs_id'];
		if(!isset($GLOBALS['_ua'][$pid]))
		{
			$pdata=get_email_adress($pid,true,true);
			if(fxIsArray($pdata) && fxIsArray($pdata[$pid]))
				$GLOBALS['_ua'][$pid]=$pdata[$pid];
			else
				$GLOBALS['_ua'][$pid]=false;
		}
		if(fxIsArray($GLOBALS['_ua'][$pid]))
			$pid_name=$GLOBALS['_ua'][$pid]['name2'];
		else
			$pid_name='('.$pid.')';

		$icon='';
		if($aa)
			$icon='<img src='.$GLOBALS['gfxpath'].'tl_neutral.png>&nbsp;&nbsp;';
		$text=meldung(3188, false, '<b>'.$pid_name.'</b>', get_entry($acd['zeitstempel'],'datum'), get_entry($acd['zeitstempel'],'zeit'));	// Erstellt/Beantragt von $var1 am $var2 um $var3 Uhr

		$aca=array(
			'pid'		=> $pid,
			'timestamp'	=> $acd['zeitstempel'],
			'text'		=> $icon.$text
		);
	}

	return $aca;
}

/**
 * ersetzt in einem String einen Teil durch einen anderen String
 * @param string $original der originale String
 * @param int $startPos die Startposition ab der ersetzt wird
 * @param int $endPos die Endposition bis zu der ersetzt wird
 * @param string $replacement der String der eingesetzt werden soll
 * @return string $original der originale String
 */
function strReplaceByPos($original, $startPos, $endPos, $replacement)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return substr($original, 0, $startPos).$replacement.substr($original, $endPos);
}

/**
 * ???FUNCTION???
 *
 * @param integer $trans - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function mg_zusatzpunkte($trans=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Zusätzlich verbrauchte Punkte pro eingebundem Zusatzfeld des Maskengenerators in Standardtransaktionen
	$punkte_pro_zusatzfeld=1;

	$mg_zusatzpunkte=array();

	$sql  = "SELECT m.nummer, m.name_maske, tm.transakt_id";
	$sql .= " FROM trans_maske tm, masken m";
	$sql .= " WHERE m.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.nummer=tm.nummer AND m.nummer<0 AND tm.transakt_id";
	if($trans)
		$sql .= "=".$trans;
	else
		$sql .= ">0";
	$tmp =db_values($sql, '*');
//fxDebug($tmp, __FILE__.' ('.__LINE__.')<br />'.$sql);
	if(is_array($tmp))
	{
		foreach($tmp as $t)
		{
			$trid=(int)$t['transakt_id'];
			if(!is_array($mg_zusatzpunkte[$trid]))
				$mg_zusatzpunkte[$trid]=array('usm'=>0, 'usf'=>0, 'usp'=>0);

			$mg_zusatzpunkte[$trid]['usm']++;

			$sql="SELECT COUNT(maskenfeldid) AS anz FROM maskenfelder WHERE nummer=".$t['nummer'];
			$anz=(int)db_value($sql);
//fxDebug($anz, __FILE__.' ('.__LINE__.')<br />'.$sql);
			if($anz)
			{
				$mg_zusatzpunkte[$trid]['usf'] += $anz;
				$mg_zusatzpunkte[$trid]['usp'] += ($anz*$punkte_pro_zusatzfeld);
			}
		}
	}
//fxDebug($mg_zusatzpunkte, __FILE__.' ('.__LINE__.') -- $mg_zusatzpunkte');

	if(!sizeof($mg_zusatzpunkte) || !$trans)
		return $mg_zusatzpunkte;
	return $mg_zusatzpunkte[$trans];
}

/**
 * hides all the forms of a given transaction, except those given in $visibles
 * @parameter int $tr the id of the transaction for which the forms should be hidden
 * @parameter array $visibles array of form-ids that should still be visible
 * @return array $nv_array holding an array of the structure ['maske{#id_of_form_}nv']=true,
 * 	which can be further processed by calling extract() with this result as first parameter
 * 		 		 
 */
function hide_forms($tr, $visibles=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// retrieve all forms of the transaction
	$call ="SELECT m.nummer";
	$call .= " FROM trans_maske tm, masken m";
	$call .= " WHERE tm.nummer=m.nummer AND tm.transakt_id=".$tr." AND m.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	if(sizeof($visibles) > 0)
		 $call .= " AND m.nummer NOT IN (".implode(', ', $visibles).")";

	$nv_array=array();
	if(is_array($form_ids=db_values($call, '*')))
	{
		foreach($form_ids as $form_id)
		{
			if($form_id['nummer'] > 0)
				$nv_array['maske'.$form_id['nummer'].'_nv']=true;
			else
				$nv_array['maske_'.abs($form_id['nummer']).'_nv']=true;
		}
	}
	return $nv_array;
}

/**
 * ???FUNCTION???
 *
 * @param various $source_msknr - Mandatory parameter:               ???PARAMETER???
 * @param various $dest_msknr   - Mandatory parameter:               ???PARAMETER???
 * @param integer $dest_line    - Optional parameter (default = 1):  ???PARAMETER???
 */
function add_usermask_to_mask($source_msknr, $dest_msknr, $dest_line=1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$source_msknr= (string)trim($source_msknr);
	$dest_msknr= (string)trim($dest_msknr);
	if(!strlen($source_msknr) || !strlen($dest_msknr))
		return;

	$sql  = "SELECT m.eingabefeld, m.anzeigeart, f.feldtyp";
	$sql .= " FROM maskenfelder m, felder f";
	$sql .= " WHERE m.nummer=".$source_msknr." AND m.anzeigeart IN (6,7,8) AND f.id=m.eingabefeld AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	$sql .= " ORDER BY m.zeile, m.spalte, m.zeilenanzahl, m.spaltenanzahl DESC";

	$mskfldarr=db_values($sql, '*');
//fxDebug($mskfldarr, $sql);
	if(is_array($mskfldarr))
	{
		foreach($mskfldarr as $mskfld)
		{
			if($dest_line <= 1)
				add_userfield_to_repmask($dest_msknr, (int)$mskfld['eingabefeld']);
			else
				add_userfield_to_listmask($dest_msknr, (int)$mskfld['eingabefeld'], $dest_line, (int)$mskfld['anzeigeart'], (int)$mskfld['feldtyp']);
		}
//fxDebug($GLOBALS['fx_mfields'][$dest_msknr], 'fx_mfields['.$dest_msknr.']');
//fxDebug($GLOBALS['fx_mfieldsarray'][$dest_msknr], 'fx_mfieldsarray['.$dest_msknr.']');
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $mnr   - Mandatory parameter: ???PARAMETER???
 * @param various $fldid - Mandatory parameter: ???PARAMETER???
 */
function add_userfield_to_repmask($mnr, $fldid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($mnr))
		$mnr=$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim($mnr));

	$fldid=(int)$fldid;
	if(!strlen($mnr) || !$fldid || !is_array($GLOBALS['fx_mfields']) && !is_array($GLOBALS['fx_mfields'][$mnr]))
		return;

	// Neuen Schlüsselwert ermitteln
	$new_key=sizeof($GLOBALS['fx_mfields'][$mnr]);

	// Neuen Spaltenindex + -nummer ermitteln
	$new_key_column= sizeof($GLOBALS['fx_mfieldsarray'][$mnr]['columns']);
	$new_column= (string)(10 + $GLOBALS['fx_mfieldsarray'][$mnr]['columns'][$new_key_column - 1]);

	// Zeile 1
	$GLOBALS['fx_mfields'][$mnr][$new_key]=array
	(
		'nummer'				=> $mnr,
		'maskenfeldid'			=> (string)(50000+$new_key),
		'zeile'					=> '1',
		'spalte'				=> $new_column,
		'anzeigeart'			=> '5',
		'eingabefeld'			=> (string)$fldid,
		'anzeigegroesse1'		=> '2',
		'anzeigegroesse2'		=> '0',
		'muss'					=> '0',
		'horausr'				=> 'left',
		'vertausr'				=> 'top',
		'spaltenanzahl'			=> '0',
		'zeilenanzahl'			=> '0',
		'spaltenbreite'			=> '',
		'zeilenhoehe'			=> '',
		'literal_overwrite'		=> '',
		'literal_value'			=> '',
		'defaultwert'			=> '0',
		'spezial'				=> '',
		'idintab'				=> '',
		'zusatzparameter_int1'	=> '',
		'zusatzparameter_int2'	=> '',
		'zusatzparameter_txt'	=> ''
	);
	$GLOBALS['fx_mfields'][$mnr][$new_key]=maske_text($GLOBALS['fx_mfields'][$mnr][$new_key]);

	// Neuen Index + Größen anpassen
	$GLOBALS['fx_mfieldsarray'][$mnr]['columns'][(string)$new_key_column]= $new_column;
	$GLOBALS['fx_mfieldsarray'][$mnr]['width'][$new_column]= '0';
	$GLOBALS['fx_mfieldsarray'][$mnr]['1'][$new_column]= 'blank';
}

/**
 * ???FUNCTION???
 *
 * @param various $mnr        - Mandatory parameter: ???PARAMETER???
 * @param various $fldid      - Mandatory parameter: ???PARAMETER???
 * @param various $dest_line  - Mandatory parameter: ???PARAMETER???
 * @param various $disp_type  - Mandatory parameter: ???PARAMETER???
 * @param various $field_type - Mandatory parameter: ???PARAMETER???
 */
function add_userfield_to_listmask($mnr, $fldid, $dest_line, $disp_type, $field_type)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($mnr))
		$mnr=$GLOBALS['_masknumber'];
	else
		$mnr=(string)(trim($mnr));

	$fldid= (int)$fldid;
	$dest_line= (string)trim($dest_line);
	$disp_type= (string)trim($disp_type);
	$field_type= (string)trim($field_type);
	if(!strlen($mnr) || !$fldid || !is_array($GLOBALS['fx_mfields']) && !is_array($GLOBALS['fx_mfields'][$mnr]))
		return;

	// Neuen Schlüsselwert ermitteln
	$new_key_literal= sizeof($GLOBALS['fx_mfields'][$mnr]);
	$new_key_field= $new_key_literal + 1;

	// Neuen Spaltenindex + -nummer ermitteln
	$new_key_column= sizeof($GLOBALS['fx_mfieldsarray'][$mnr]['columns']);
	$new_column= (string)(10 + $GLOBALS['fx_mfieldsarray'][$mnr]['columns'][$new_key_column - 1]);

	// Zeile 1 und 10: Literal + Auswahlfeld vom Projektleiterfeld kopieren und anpassen
	for($i=0; $i < 2; $i++)
	{
		if($i == 0)	// Zeile 1
		{
			$new_key= $new_key_literal;
			$line= '1';
			$disp= '3';
			$size1= '0';
			$size2= '0';
		}
		else if($dest_line != '1')	// Zeile x
		{
			$new_key= $new_key_field;
			$line= $dest_line;
			$disp= $disp_type;
			$size1= '0';
			$size2= '0';

//echo('$fldid='.$fldid.', $disp='.$disp.', $field='.$field_type.'<br />');
			if($disp == '8')			// Select field
				$size2='120';
			else if($disp == '7')		// Input field (multi lines)
			{
				$size1='-200';
				$size2='-22';
			}
			else						// Input field (single line)
			{
				if($field_type == '7')	// Date
					$size1='-60';
				else
					$size1='-150';
			}
		}
		else
			continue;

		// Zeile
		$GLOBALS['fx_mfields'][$mnr][$new_key]=array
		(
			'nummer'				=> $mnr,
			'maskenfeldid'			=> (string)(50000+$new_key),
			'zeile'					=> $line,
			'spalte'				=> $new_column,
			'anzeigeart'			=> $disp,
			'eingabefeld'			=> (string)$fldid,
			'anzeigegroesse1'		=> $size1,
			'anzeigegroesse2'		=> $size2,
			'muss'					=> '0',
			'horausr'				=> 'left',
			'vertausr'				=> 'middle',
			'spaltenanzahl'			=> '0',
			'zeilenanzahl'			=> '0',
			'spaltenbreite'			=> '',
			'zeilenhoehe'			=> '',
			'literal_overwrite'		=> '',
			'literal_value'			=> '',
			'defaultwert'			=> '0',
			'spezial'				=> '',
			'idintab'				=> '',
			'zusatzparameter_int1'	=> '',
			'zusatzparameter_int2'	=> '',
			'zusatzparameter_txt'	=> ''
		);
		$GLOBALS['fx_mfields'][$mnr][$new_key]=maske_text($GLOBALS['fx_mfields'][$mnr][$new_key]);
	}

	// Neuen Index + Größen in maskenfeld_array anpassen
	$GLOBALS['fx_mfieldsarray'][$mnr]['columns'][(string)$new_key_column]= $new_column;
	$GLOBALS['fx_mfieldsarray'][$mnr]['width'][$new_column]= '0';
	$GLOBALS['fx_mfieldsarray'][$mnr]['1'][$new_column]= 'blank';
	$GLOBALS['fx_mfieldsarray'][$mnr][$dest_line][$new_column]= 'blank';
	$GLOBALS['fx_mfieldsarray'][$mnr]['9999'][$new_column]= 'blank';
}

/**
 * Retrieves the fields that were added by the user into the process detail transaction through user definded forms
 */
function get_user_form_fields()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql  = "SELECT name_maske, f.feldname, feldtyp FROM trans_maske tm, masken m, maskenfelder mf, felder f";
	$sql .= " WHERE m.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND tm.nummer=mf.nummer AND m.nummer=tm.nummer";
	$sql .= " AND mf.eingabefeld=f.id AND tm.transakt_id=23 AND tm.nummer<0 AND mf.anzeigeart>5 AND mf.anzeigeart<12";
	$sql .= " AND f.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";

	return db_values($sql, '*');
}

/**
 * LOAD_PDFCONFIG
 *
 * @param various $modus   - Mandatory parameter:                ???PARAMETER???
 * @param string  $selname - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function load_pdfconfig($modus, $selname='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl_b;
//$GLOBALS['load_pdf_config_debug']=true;

	// Mandanten-Ordner
	$cd=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'];

	// "Automatisch" immer für Leistungsnachweis (modus=1), Rechnung (modus=2) oder Personalstammblatt (Ext. MA=204 + Int. MA=205)
	$fitfix=0;
	if(($modus == 1) || ($modus == 2) || ($GLOBALS['fxpglobals']['tr'] == 204) || ($GLOBALS['fxpglobals']['tr'] == 205))
		$fitfix=1;

	// Standard-Werte
	$cfg_array=array
	(
		'modus'		=> $modus,			// Aktueller Konfigurationsmodus
		'width'		=> (float)595.0,	// Breite des PDF-Dokuments	(596=210 mm)
		'height'	=> (float)842.0,	// Höhe des PDF-Dokuments	(842=297 mm)
		'lmargin'	=> (float)20.0,		// Linker Rand				(20=7 mm)
		'rmargin'	=> (float)20.0,		// Rechter Rand				(20=7 mm)
		'tmargin'	=> (float)20.0,		// Oberer Rand				(20=7 mm)
		'bmargin'	=> (float)20.0,		// Unterer Rand				(20=7 mm)
		'fitfix'	=>	$fitfix,
		'fit'		=>	0,
	);
//fxDebug('_GET');

	// Minimum-Werte (in mm)
	$min_width=210.0;
	$min_height=297.0;

	// Formular
	$ffilename=$cd.'/'.substr('000000000'.$GLOBALS['fxpglobals']['person'], -9).substr('0000'.$_POST['tr'], -4).'.cfg';
//fxDebug($ffilename,'$ffilename');
	if(fxIsArray($_GET) && $_GET['pdfs_button'] && isset($_GET['pdfs_form']))
	{
		if((strlen($_GET['pdfs_form']) < 8) && file_exists($ffilename))
			fxDelete($ffilename);
		else
		{
			// Subordner mit Mandantennumer erstellen, falls noch nicht vorhanden
			if(!is_dir($cd))
				mkdir($cd, 0755);
			fxSave($ffilename, $_GET['pdfs_form'].$nl_b);
		}
	}
	$fname=trim(fxLoad($ffilename));
//fxDebug($fname,'$fname');
	if(strlen($fname) && !file_exists($fname))
		$fname='';
	if(!strlen($selname) && strlen($fname))
		$selname=$fname;
//fxDebug($selname,'$selname');

	if(!isset($GLOBALS['_preview_config']))
	{
		// Konfigurationsfile ermitteln
		$filename='';
		if(strlen($selname))
		{
			$filename=$selname;
			while(true)
			{
				$sp=strpos($filename, '/');
				if($sp !== false)
					$filename=substr($filename, $sp+1);
				else
					break;
			}
		}
		if(!strlen($filename))
		{
			if($modus == -3)		// Netzplandiagramm
				$filename='cfg_pnzp.cfg';
			else if($modus == -2)	// Projektstrukturplan
				$filename='cfg_ppsp.cfg';
			else if($modus == -1)	// Projektstatus/Urlaubsplan/Gantt
				$filename='cfg_ppst.cfg';
			else if($modus == 1)	// Leistungsnachweis
				$filename='cfg_plnw.cfg';
			else if($modus == 2)	// Rechnung
				$filename='cfg_prng.cfg';
			else if($modus == 3)	// Reisekostenreports
				$filename='cfg_prkr.cfg';
			else					// Standardreport
				$filename='cfg_pstd.cfg';
		}
//fxDebug($filename,'$filename');

		$bfilename_special=substr($filename, 0, -4).'_'.substr('000000000'.$GLOBALS['fxpglobals']['person'], -9).substr('0000'.$_POST['tr'], -4).'.cfg';
//fxDebug($bfilename_special,'$bfilename_special');
		$bfilename=substr($filename, 0, 8).'_'.substr('000000000'.$GLOBALS['fxpglobals']['person'], -9).substr('0000'.$_POST['tr'], -4).'.cfg';
//fxDebug($bfilename,'$bfilename');

		// Benutzerdefinierte Einstellungen zwischenspeichern
		if($_GET['pdfs_button'])
		{
			if($GLOBALS['load_pdf_config_debug'])
				fxDebug($_POST, 'Benutzerdefinierte Einstellungen zwischenspeichern');

			// Subordner mit Mandantennumer erstellen, falls noch nicht vorhanden
			if(!is_dir($cd))
				mkdir($cd, 0755);

			// Einstellungsfile erstellen
			$pdffac=0.35273159145;
			$tfilename=$cd.'/'.$bfilename_special;
//fxDebug($tfilename,'$tfilename');
			$sc='';

			// Format
			$dw=trim($_GET['pdfs_format']);
			if(strlen($dw) && ($dw != '0x0'))
			{
				$trz=strpos($dw, 'x');
				$_GET['pdfs_width']=get_double(substr($dw, 0, $trz));
				$_GET['pdfs_height']=get_double(substr($dw, $trz+1));
			}
			// Seitenbreite
			$dw=get_double($_GET['pdfs_width']);
			if($dw > 0.00)
			{
				$dw=MAX($dw, $min_width);
				$sc .= '#_width		'.($dw/$pdffac).$nl_b;
			}
			// Seitenhöhe
			$dw=get_double($_GET['pdfs_height']);
			if($dw > 0.00)
			{
				$dw=MAX($dw, $min_height);
				$sc .= '#_height	'.($dw/$pdffac).$nl_b;
			}
			// Rand links
			$dw=get_double($_GET['pdfs_lmargin']);
			if($dw > 0.00)
			{
				$dw=MAX($dw, 7.00);
				$sc .= '#_lmargin	'.($dw/$pdffac).$nl_b;
			}
			// Rand rechts
			$dw=get_double($_GET['pdfs_rmargin']);
			if($dw > 0.00)
			{
				$dw=MAX($dw, 7.00);
				$sc .= '#_rmargin	'.($dw/$pdffac).$nl_b;
			}
			// Rand oben
			$dw=get_double($_GET['pdfs_tmargin']);
			if($dw > 0.00)
			{
				$dw=MAX($dw, 7.00);
				$sc .= '#_tmargin	'.($dw/$pdffac).$nl_b;
			}
			// Rand unten
			$dw=get_double($_GET['pdfs_bmargin']);
			if($dw > 0.00)
			{
				$dw=MAX($dw, 7.00);
				$sc .= '#_bmargin	'.($dw/$pdffac).$nl_b;
			}
			// Skalierungsfaktor
			$dw=get_double($_GET['pdfs_fac']);
			if($dw > 0.00)
			{
				$dw=MIN(MAX($dw, $_GET['pdfs_minfac']), $_GET['pdfs_maxfac']);
				$sc .= '#_fac		'.($dw/100.00).$nl_b;
			}
			// Automatisch
			if($_GET['pdfs_fit'])
				$sc .= '#_fit		1'.$nl_b;
			else
				$sc .= '#_fit		0'.$nl_b;
			// Querformat
			if($_GET['pdfs_lsfmt'])
				$sc .= '#_lsfmt		1'.$nl_b;
			else
				$sc .= '#_lsfmt		0'.$nl_b;
			// In Graustufen
			if($_GET['pdfs_grey'])
				$sc .= '#_grey		1'.$nl_b;
			else
				$sc .= '#_grey		0'.$nl_b;
			// Legende
			if($_GET['pdfs_leg_exists'])
			{
				if($_GET['pdfs_leg'])
					$sc .= '#_leg		1'.$nl_b;
				else
					$sc .= '#_leg		0'.$nl_b;
			}
			// Anhang
			if($_GET['pdfs_att_exists'])
			{
				if($_GET['pdfs_att'])
					$sc .= '#_att		1'.$nl_b;
				else
					$sc .= '#_att		0'.$nl_b;
			}

			fxSave($tfilename, $sc);
		}

		// Konfigurationsfile(s) einlesen und parsen
		$nfilename=substr($filename,0,8).'.cfg';
		$tfn_arr=array(
			$selname,								// 1. Versuch: Übergabe-Konfigurationsfile
			$cd.'/'.$filename,						// 2. Versuch: Mandantenordner/Spezialkonfiguration
			$cd.'/'.$nfilename,						// 3. Versuch: Mandantenordner/Normalkonfiguration
			$GLOBALS['datpath'].'0/'.$filename,		// 4. Versuch: Standardordner/Spezialkonfiguration
			$GLOBALS['datpath'].'0/'.$nfilename,	// 5. Versuch: Standardordner/Normalkonfiguration
		);
		if($nfilename != 'cfg_pstd.cfg')
		{
			$tfn_arr[]=$cd.'/cfg_pstd.cfg';						// 6. Versuch: Mandantenordner/Standardkonfiguration
			$tfn_arr[]=$GLOBALS['datpath'].'0/cfg_pstd.cfg';	// 7. Versuch: Standardordner/Standardkonfiguration
		}
//fxDebug($tfn_arr,'$tfn_arr');

		$fn_arr=array();
		foreach($tfn_arr as $t)
		{
			$fnd=false;
			if(sizeof($fn_arr))
			{
				foreach($fn_arr as $f)
				{
					if($f == $t)
					{
						$fnd=true;
						break;
					}
				}
			}
			if(!$fnd)
				$fn_arr[]=$t;
		}
//fxDebug($fn_arr, '$fn_arr');

		foreach($fn_arr as $fn)
		{
			if($GLOBALS['load_pdf_config_debug'])
				echo('<b>Konfigurationsfile einladen:</b> "'.$fn.'"...');

			$f=@file($fn);
			if(is_array($f))
			{
				if($GLOBALS['load_pdf_config_debug'])
					echo(' <b><font color="#009900">ERFOLG.</font></b><br /><br />'.$GLOBALS['nl']);

				break;
			}

			if($GLOBALS['load_pdf_config_debug'])
				echo(' <b><font color="#AA0000">FEHLER!</font></b><br />'.$GLOBALS['nl']);
		}
	}
	else
		$f=$GLOBALS['_preview_config'];

	// File erfolgreich eingeladen, dann folgt hier die Auswertung
	$tfilename='';
	if(is_array($f))
	{
		$nextfile=true;
		while($nextfile)
		{
			$param=false;
			$zc=0;
			foreach($f as $z)
			{
				$zc++;

				if($GLOBALS['load_pdf_config_debug'])
					echo('<b>Zeile '.$zc.':</b> '.$z.'<br />&nbsp;&nbsp;&nbsp;-> ');

				$kp=strpos($z, '//');
				if($kp !== false)	// Kommentare löschen
					$z=trim(substr($z, 0, $kp));
				else
					$z=trim($z);
				if(!strlen($z))	// Leerzeilen ignorieren
				{
					if($GLOBALS['load_pdf_config_debug'])
						echo('<b><font color="#AA0000">ignorieren</font></b><br />'.$GLOBALS['nl']);

					continue;
				}

				if($GLOBALS['load_pdf_config_debug'])
					echo('<b><font color="#009900">'.$z.'</font></b> ('.$kp.')<br />'.$GLOBALS['nl']);

				if(substr($z, 0, 1) == '#')	// Neuen Parameter gefunden
				{
					$param='';
					$value='';
					$vmode=0;
					for($s=1; $s<strlen($z); $s++)
					{
						$c=substr($z, $s, 1);
						if(ord($c) <= 32)	// Whitespace
						{
							if($vmode > 1)
								$value .= $c;
							else
								$vmode=1;
						}
						else
						{
							if($vmode == 1)
								$vmode=2;
							if($vmode)
								$value .= $c;	// Neues Zeichen zum Wert hinzufügen
							else
								$param .= $c;	// Neues Zeichen zum Parameter hinzufügen
						}
					}
					$param=trim(strtolower($param));
					if(substr($param, 0, 1) == '_')
					{
						str_replace(',', '.', $value);
						$param=substr($param, 1);
						$cfg_array[$param]=(float)$value;
					}
					else
						$cfg_array[$param]=$value;

					if($GLOBALS['load_pdf_config_debug'])
						echo('&nbsp;&nbsp;&nbsp;<b><font color="#000099">NEUER Parameter:</font> '.$param.'</b> -- <b><font color="#000099">Value:</font> '.fxHtmlEncode($cfg_array[$param]).'</b> (Typ: '.gettype($cfg_array[$param]).')<br />'.$GLOBALS['nl']);
				}
				else if($param)
				{
					$cfg_array[$param] .= $z;

					if($GLOBALS['load_pdf_config_debug'])
						echo('&nbsp;&nbsp;&nbsp;<b><font color="#000099">Parameter:</font> '.$param.'</b> -- <b><font color="#000099">Value:</font> '.fxHtmlEncode($cfg_array[$param]).'</b> (Typ: '.gettype($cfg_array[$param]).')<br />'.$GLOBALS['nl']);
				}
			}

			// Persönliches Einstellungsfile einladen
			if(!strlen($tfilename))
			{
				if($GLOBALS['load_pdf_config_debug'])
					echo('<hr />'.$GLOBALS['nl']);
				$tfilename=$cd.'/'.$bfilename_special;
				if(!file_exists($tfilename))
				{
					if($GLOBALS['load_pdf_config_debug'])
						echo('<b>Konfigurationsfile einladen:</b> "'.$tfilename.'"... <b><font color="#AA0000">FEHLER!</font></b><br />'.$GLOBALS['nl']);
					$tfilename=$cd.'/'.$bfilename;
				}
				if($GLOBALS['load_pdf_config_debug'])
					echo('<b>Konfigurationsfile einladen:</b> "'.$tfilename.'"...');
				$f=false;
				if(file_exists($tfilename))
					$f=@file($tfilename);
				if(is_array($f))
				{
					if($GLOBALS['load_pdf_config_debug'])
						echo(' <b><font color="#009900">ERFOLG.</font></b><br /><br />'.$GLOBALS['nl']);
				}
				else
				{
					if($GLOBALS['load_pdf_config_debug'])
						echo(' <b><font color="#AA0000">FEHLER!</font></b><br />'.$GLOBALS['nl']);

					$nextfile=false;
				}
			}
			else
				$nextfile=false;
		}
	}

	// Verkleinerungs-/Vergrößerungsfaktor
	if(!$cfg_array['fac'])
		$cfg_array['fac']=1.00;

	// Originalwerte zwischenspeichern
	$mm2pt=2.835016835006757;
	$min_width_pt=$min_width*$mm2pt;
	$min_height_pt=$min_height*$mm2pt;
	if($cfg_array['width'] < $min_width_pt)			// Mindestseitenbreite
		$cfg_array['width']=$min_width_pt;
	if($cfg_array['height'] < $min_height_pt)		// Mindestseitenhöhe
		$cfg_array['height']=$min_height_pt;
	$cfg_array['owidth']=$cfg_array['width'];
	$cfg_array['oheight']=$cfg_array['height'];

	// Konfigurationsfile prüfen und gegebenenfalls wichtige Werte ersetzen
	if(isset($cfg_array['lsfmt']) && (($cfg_array['lsfmt'] && ($cfg_array['height'] > $cfg_array['width'])) || (!$cfg_array['lsfmt'] && ($cfg_array['width'] > $cfg_array['height']))))	// Querformat/Hochformat
	{
		$swap=$cfg_array['width'];
		$cfg_array['width']= $cfg_array['height'];
		$cfg_array['height']= $swap;
	}
	if($cfg_array['lmargin'] < 19.8451178451)		// Linker Rand muss mind. 19.8451178451=7 mm sein
		$cfg_array['lmargin']=19.8451178451;
	if($cfg_array['rmargin'] < 19.8451178451)		// Rechter Rand muss mind. 19.8451178451=7 mm sein
		$cfg_array['rmargin']=19.8451178451;
	if($cfg_array['tmargin'] < 19.8451178451)		// Oberer Rand muss mind. 19.8451178451=7 mm sein
		$cfg_array['tmargin']=19.8451178451;
	if($cfg_array['bmargin'] < 19.8451178451)		// Unterer Rand muss mind. 19.8451178451=7 mm sein
		$cfg_array['bmargin']=19.8451178451;
	if($cfg_array['fac'] < 0.25)					// Verkleinerungsfaktor darf nicht kleiner als 0.25 sein
		$cfg_array['fac']=0.25;
	if($cfg_array['fac'] > 5.00)					// Vergrösserungsfaktor darf nicht grösser als 5.0 sein
		$cfg_array['fac']=5.00;
	$cfg_array['ofac']=$cfg_array['fac'];

	// Kopfhöhe muss mind. 280=100 mm sein, falls Adresse angezeigt werden soll
	if(strpos($orderline, '^!adr') !== false)
	{
		if(($cfg_array['h_height'] + $cfg_array['tmargin']) < 257.0)
			$cfg_array['h_height']=(float)(257.0 - $cfg_array['tmargin']);
	}

	// Automatisch
	if($cfg_array['fitfix'])
		$cfg_array['fit']=1;
	if($cfg_array['fit'])
		$cfg_array['fac']=1.00;

	// Formular
	$cfg_array['form']=$fname;
//fxDebug($cfg_array,'$cfg_array');

	// Konfigurationsfile zurückgeben
	return $cfg_array;
}

/**
 * ???FUNCTION???
 *
 * @param various $mode       - Mandatory parameter:                   ???PARAMETER???
 * @param various $mid        - Optional parameter (default = -1):     ???PARAMETER???
 * @param boolean $with_inact - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function select_pdfconfig($mode, $mid=-1, $with_inact=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lin=' {'.get_text(1391, '20').'}';	// {inaktiv}
	$mode=(int)$mode;
	$mode_arr=array(-1=>'pst', 0=>'std', 'lnw', 'rng', 'rkr');
	$sel_arr=array();
	$fn=$mode_arr[$mode];

	if(!$fn)
		$fn=$mode_arr[0];

	$prefix='cfg_p'.$fn;
	$suffix='.cfg';
//echo('$mode='.$mode.', $prefix=['.$prefix.'], $suffix=['.$suffix.']<br />'.$GLOBALS['nl']);

	if($mid < 0)
		$basedir_arr=array($GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'], $GLOBALS['datpath'].'0');
	else
		$basedir_arr=array($GLOBALS['datpath'].$mid);
	foreach($basedir_arr as $basedir)
	{
		if(@is_dir($basedir))
		{
			$dh=opendir($basedir);
			if($dh)
			{
				while(($fname=readdir($dh)) !== false)
				{
					$filename=$basedir.'/'.$fname;
					$ftype=filetype($filename);
					$fstatus=stat($filename);

					if(($ftype == 'file') && (substr($fname,0,8) == $prefix) && (substr($fname,-4) == $suffix))
					{
						$inact=false;
						$wname=$fname;
						if(substr($fname,-5,1) == '!')
						{
							$inact=true;
							$wname=substr($wname,0,-5).substr($wname,-4);
						}
						$us='';
						if(strlen($wname) > 25)
							$us=substr($wname,-18,1);
						if($us == '_')
						{
							for($up=-17; $up<-4; $up++)
							{
								$c=substr($wname,$up,1);
								if(!is_numeric($c))
								{
									$us='';
									break;
								}
							}
						}
//echo('$fname='.$fname.', $wname='.$wname.', $inact='.$inact.', $us='.$us.'<br />');
						if((!$inact || $with_inact) && !strlen($us))
						{
//echo('-&gt; <b>'.$wname.'</b><hr />');
							$sp=strpos($basedir,'/');
							if($sp === false)
								$sp=-1;
							$name=substr('000000000'.substr($basedir,$sp+1), -strlen($GLOBALS['fxpglobals']['client'])).': ';
							if(strlen($wname) == 12)
								$name .= '(Standard)';
							else
							{
								// Read file
								$f=fxLoad($filename);
//echo('<pre>'.fxHtmlEncode($f).'</pre>'.$GLOBALS['nl']);
								$anp=strpos($f, '#form');
								if($anp !== false)
								{
									$enp=strpos($f, "\n", $anp+6);
									$name .= strtr(trim(substr($f, $anp+6, $enp-$anp-6)), $GLOBALS['caption_strtr']);
								}
								else
									$name .= substr($wname,8,-4);
								if($inact)
									$name .= $lin;
							}
//echo('-&gt; <b>'.$name.'</b><br />'.$GLOBALS['nl']);

							$sel_arr[$filename]=$name;
						}
					}
				}

				if($basedir && sizeof($sel_arr))
					asort($sel_arr);
			}
		}
	}

	return $sel_arr;
}

/**
 * Alle Überstundenauszahlungen dieser Person(en) im gewählten Zeitraum ermitteln
 *
 * @param various $pid       - Mandatory parameter: ???PARAMETER???
 * @param various $startdate - Mandatory parameter: ???PARAMETER???
 * @param various $enddate   - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function overtime_outpayment($pid, $startdate, $enddate)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid= trim($pid);
	$startdate= substr($startdate, 0, 8).'000000';
	$enddate= substr($enddate, 0, 8).'240000';
	$mand_id= $GLOBALS['fxpglobals']['client'];
	$oop= array();

	$sql  = "SELECT startzeit, stunden, bestaetigung_anmerk FROM termine";
	$sql .= " WHERE terminart=".FXP_AT_OVERTIMEOUTPAY." AND personen_id IN (".$pid.") AND mandanten_id=".$mand_id." AND (startzeit<='".$enddate."' AND startzeit>='".$startdate."')";
	$sql .= " ORDER BY startzeit";
	$tmp =db_values($sql, '*');
	if(is_array($tmp))
	{
		foreach($tmp as $t)
		{
			$day=substr($t['startzeit'], 0, 8);
			if(isset($oop[$day]))
			{
				$oop[$day]['sec'] += (int)($t['stunden']*3600);
				$oop[$day]['txt'] .= ' -- '.substr($t['bestaetigung_anmerk'], 2);
			}
			else
				$oop[$day]=array('sec'=>(int)($t['stunden']*3600), 'txt'=>substr($t['bestaetigung_anmerk'], 2));
		}
	}
//fxDebug($oop, $sql);

	return $oop;
}

/**
 * ???FUNCTION???
 *
 * @param various $termin_id  - Mandatory parameter: ???PARAMETER???
 * @param various $data_array - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function gleittag($termin_id, $data_array)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$termin_id=(int)$termin_id;
	$mand_id=$GLOBALS['fxpglobals']['client'];

	// Prüfen, ob Gleittageintrag existiert
	$sql  = "SELECT z.personen_id, z.zeit_id, z.termin_id, z.ist_beginn, z.ist_ende, z.kw, z.pause, z.zeitaufwand, t.taetigkeit_id";
	$sql .= " FROM zeitdaten z, taetigkeiten t";
	$sql .= " WHERE z.projekt_id=-1 AND z.zeitart=".FXP_TE_TIMEREC." AND z.termin_id=".$termin_id." AND t.zeit_id=z.zeit_id AND t.mandanten_id=z.mandanten_id AND z.mandanten_id=".$mand_id;
	$old =db_values($sql);

	$err=false;
	// ...Nein, dann FXP_CREATE
	if(!fxIsArray($old))
	{
		$err=1948;	// Der [<b>Gleittag</b>] konnte nicht angelegt werden!
		if(fxIsArray($data_array))
		{
//fxDebug($data_array, '$data_array');
			$zeit_id= id_max('zeitdaten', 'zeit_id');
			$taetigkeit_id= id_max('taetigkeiten', 'taetigkeit_id');

			$zeit_arr=array(
				'mandanten_id'		=> $mand_id,
				'zuordnungs_mid'	=> $mand_id,
				'personen_id'		=> (int)$data_array['personen_id'],
				'zeit_id'			=> $zeit_id,
				'projekt_id'		=> -1,
				'mutterprojekt_id'	=> 0,
				'termin_id'			=> $termin_id,
				'ist_beginn'		=> "'".$data_array['ist_beginn']."'",
				'ist_ende'			=> "'".$data_array['ist_ende']."'",
				'kw'				=> kw($data_array['ist_beginn']),
				'pause'				=> (int)$data_array['pause'],
				'zeitaufwand'		=> (int)$data_array['zeitaufwand'],
				'zeitart'			=> FXP_TE_TIMEREC
			);
//fxDebug($zeit_arr, '$zeit_arr');

			$sk='';
			$sv='';
			foreach($zeit_arr as $zk => $zv)
			{
				$sk .= ",".$zk;
				$sv .= ",".$zv;
			}
			$sql="INSERT INTO zeitdaten (".substr($sk, 1).") VALUES (".substr($sv, 1).")";
			$ser=db_x($sql);
//fxDebug($ser, $sql);

			if(!$ser)
			{
				$taetigkeit_arr=array(
					'mandanten_id'		=> $mand_id,
					'taetigkeit_id'		=> $taetigkeit_id,
					'personen_id'		=> (int)$data_array['personen_id'],
					'zeit_id'			=> $zeit_id,
					'projekt_id'		=> -1,
					'taetigkeit_text'	=> "'".meldung(1525, false)."'"	// [Gleittag]
				);
//fxDebug($taetigkeit_arr, '$taetigkeit_arr');

				$sk='';
				$sv='';
				foreach($taetigkeit_arr as $zk => $zv)
				{
					$sk .= ",".$zk;
					$sv .= ",".$zv;
				}
				$sql="INSERT INTO taetigkeiten (".substr($sk, 1).") VALUES (".substr($sv, 1).")";
				$ser=db_x($sql);
//fxDebug($ser, $sql);
			}

			if(!$ser)
				$err=false;
		}
		else
			$err=false;
	}
	else
	{
		$old['ist_beginn']= "'".$old['ist_beginn']."'";
		$old['ist_ende']= "'".$old['ist_ende']."'";

		$zeit_id= (int)$old['zeit_id'];
		$taetigkeit_id= (int)$old['taetigkeit_id'];

		// ...Ja, dann FXP_CHANGE, falls Datenarray mitgegeben wurde
		if(fxIsArray($data_array))
		{
			$err=1949;	// Der [<b>Gleittag</b>] konnte nicht geändert werden!

			$zeit_arr=array(
				'personen_id'		=> (int)$data_array['personen_id'],
				'ist_beginn'		=> "'".$data_array['ist_beginn']."'",
				'ist_ende'			=> "'".$data_array['ist_ende']."'",
				'kw'				=> kw($data_array['ist_beginn']),
				'pause'				=> (int)$data_array['pause'],
				'zeitaufwand'		=> (int)$data_array['zeitaufwand']
			);
//fxDebug($zeit_arr, '$zeit_arr');

			$change=false;
			$sc='';
			foreach($zeit_arr as $zk => $zv)
			{
				if($old[$zk] != $zv)
					$change=true;

				$sc .= ",".$zk."=".$zv;
			}

			if($change)
			{
				$sql="UPDATE zeitdaten SET ".substr($sc, 1)." WHERE zeit_id=".$zeit_id." AND mandanten_id=".$mand_id;
				$ser=db_x($sql);
//fxDebug($ser, $sql);
			}

			if(!$ser)
				$err=false;
		}

		// ...Sonst, FXP_DELETE
		else
		{
			$err=1950;	// Der [<b>Gleittag</b>] konnte nicht geändert werden!

			$sql="DELETE FROM zeitdaten WHERE zeit_id=".$zeit_id." AND mandanten_id=".$mand_id;
			$ser=db_x($sql);
//fxDebug($ser, $sql);
			if(!$ser)
			{
				$sql="DELETE FROM taetigkeiten WHERE taetigkeit_id=".$taetigkeit_id." AND mandanten_id=".$mand_id;
				$ser=db_x($sql);
//fxDebug($ser, $sql);
			}

			if(!$ser)
				$err=false;
		}
	}

	return $err;
}

/**
 * ???FUNCTION???
 *
 * @param various $at - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getActivityTypes($at)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Farben
	$col_no= '#888888';
	$col_prod= '#44aa55';
	$col_nprod= '#bb4455';

	$acttypes=array(
		-1 => '&nbsp;',													// leer
		0  => '<i style="color:'.$col_no.';">('.meldung(388).')</i>'	// (ohne)
	);

	if(fxIsArray($at))
		$at=array_to_csv($at);
	if(is_string($at))
	{
		$at=trim($at);
//fxDebug($at, '$at');

		if(strlen($at))
		{
			$cset=false;
			if(!isset($GLOBALS['dbconv']))
			{
				$GLOBALS['dbconv']=true;
				$cset=true;
			}
			$sql  = "SELECT kostenstelle_id, kostennummer, kostenbezeichnung, produktiv";
			$sql .= " FROM kostenstellen";
			$sql .= " WHERE kostenstelle_id IN (".$at.") AND kostendefinition=903 AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND id_sprache=".$GLOBALS['fxpglobals']['lang'];
			$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
			if($cset)
				unset($GLOBALS['dbconv']);

			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					$col=$col_nprod;
					if($t['produktiv'])
						$col=$col_prod;

					$lart='<font style="color:'.$col.';">';
					if(strlen($t['kostenbezeichnung']))
						$lart .= '<b>'.$t['kostenbezeichnung'].'</b> ';
					$lart .= '['.$t['kostennummer'].']</font>';

					$acttypes[(int)$t['kostenstelle_id']]=$lart;
				}
			}
		}
	}

	return $acttypes;
}

/**
 * ???FUNCTION???
 *
 * @param various $searchname - Mandatory parameter:                ???PARAMETER???
 * @param string  $timestamp  - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getDocuments($searchname, $timestamp='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$docs=array();

	// Dokument suchen
	$sql  = "SELECT dok_id, dok_originalname, elter_art, elter, anlage_dtm, zeitstempel FROM d_dokumente";
	$sql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND dok_originalname LIKE '".cleanFilename($searchname,true)."'";
	if(strlen($timestamp))
		$sql .= " AND zeitstempel>='".substr($timestamp, 0, 8)."000000'";
	$sql .= " ORDER BY zeitstempel DESC";
	$dat =db_values($sql, '*');
//fxDebug($dat, $sql);
	if(fxIsArray($dat))
	{
		foreach($dat as $d)
		{
			$sql  = "SELECT datei_name, version_nr, dok_verlauf_id FROM d_dok_verlauf";
			$sql .= " WHERE dok_id=".$d['dok_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY version_nr DESC, dok_verlauf_id DESC";
			$vdt =db_values($sql, '*');
//fxDebug($vdt, $sql);
			if(fxIsArray($vdt))
			{
				foreach($vdt as $v)
				{
					$fn=$GLOBALS['fxpglobals']['settings']['dok_archiv'].$GLOBALS['fxpglobals']['client'].substr($GLOBALS['fxpglobals']['settings']['dok_archiv'], -1).$v['datei_name'];
//fxDebug(file_exists($fn), 'file_exists('.$fn.')');
					if(file_exists($fn))
						$docs[]=$d['dok_id'].'|'.$fn;
				}
			}
		}
	}

	return $docs;
}

/**
 * ???FUNCTION???
 *
 * @param various $mn  - Mandatory parameter: ???PARAMETER???
 * @param various $ids - Mandatory parameter: ???PARAMETER???
 * @param various $sel - Mandatory parameter: ???PARAMETER???
 * @param various $def - Mandatory parameter: ???PARAMETER???
 * @param various $all - Mandatory parameter: ???PARAMETER???
 */
function getColumnSelects($mn,$ids,$sel,$def,$all)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('$sel='.$sel.'<hr />$def='.$def.'<hr />$all='.$all.'<hr />');
//fxDebug($ids,'$ids');
	// ...Adjust def + all
	if(strlen($sel))
	{
		$ndef='';
		$nall='';
		for($c=0; $c<strlen($sel); $c++)
		{
			$s=(int)substr($sel,$c,1);
			if($s > 2)
			{
				$d=$s;
				$a=$s;
				if(fxIsArray($ids) && fxIsArray($ids[$c]))
					$ids[$c]['def']=$s;
			}
			else
			{
				$d=(int)substr($def,$c,1);
				$a=1;
			}
			$ndef .= $d;
			$nall .= $a;
		}
		$def=$ndef;
		$all=$nall;
//echo('$ndef='.$ndef.'<hr />$nall='.$nall.'<hr />');
//fxDebug($ids,'$ids');
	}
	$cva=array();
	$sval='';
	// ...Default
	if(!strlen($sval) && ($sel === $def))
		$sval='0_'.$def;
	$cva['0_'.$def]='[Default]';
	// ...All
	if(!strlen($sval) && ($sel === $all))
		$sval='1_'.$all;
	$cva['1_'.$all]='['.meldung(1422, true).']';
	// ...User
	if(fxIsArray($ids))
	{
//echo('<u>$sval</u><br />'.$sval.'<hr />');
		$sql="SELECT personen_id FROM benutzer WHERE benutzer_id=1 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$adm_persid=(int)db_value($sql);
//fxDebug($adm_persid, $sql);

		$mand0='';
		if(strtolower(substr($GLOBALS['fxpglobals']['settings']['fxp_spalten'], 0, 1)) == 'j')
			$mand0="mandanten_id=0 OR ";

		$sql  = "SELECT mandanten_id, personen_id, feldname, einstellungswert, zeitstempel";
		$sql .= " FROM reportspalten";
		$sql .= " WHERE (".$mand0."(mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (personen_id=".$GLOBALS['fxpglobals']['person'];
		if($adm_persid && ($adm_persid != $GLOBALS['fxpglobals']['person']))
			$sql .= " OR personen_id=".$adm_persid;
		$sql .= "))) AND trans_id=".$GLOBALS['fxpglobals']['tr']." AND nummer=".$mn." AND (id_sprache IS NULL OR id_sprache=".$GLOBALS['fxpglobals']['lang'].")";
		$sql .= " ORDER BY mandanten_id, personen_id, feldname";
		$ust=db_values($sql, '*');
//fxDebug($ust, $sql);
		if(fxIsArray($ust))
		{
			foreach($ust as $u)
			{
				$selids=explode('_', $u['einstellungswert']);
				if(!fxIsArray($selids))
					continue;

				$actids=array();
				foreach($selids as $id)
					$actids[(int)$id]=true;

				if(!$u['mandanten_id'])
					$u['personen_id']=0;
				$ev=$u['zeitstempel'].'-'.$u['personen_id'].'_';
				foreach($ids as $i)
				{
					$df=(int)$i['def'];
					if($df > 1)
						$ev .= $df;
					else if($actids[$i['id']])
						$ev .= '1';
					else
						$ev .= '0';
				}

				$oname=$u['feldname'];
				if(!$u['personen_id'])
					$oname='[FXP] '.$oname;
				else if($u['personen_id'] != $GLOBALS['fxpglobals']['person'])
					$oname='[Admin] '.$oname;
				$uep=strpos($ev, '_');
				$cev=substr($ev, $uep+1);
				if(!strlen($sval) && ($sel === $cev))
				{
					$sval=$ev;
//echo('<u><b>'.$oname.'</b></u><br />'.$sel.'<br />'.$cev.'<hr />');
				}
				else
				{
//echo('<u>'.$oname.'</u><br />'.$sel.'<br />'.$cev.'<hr />');
				}
				$cva[$ev]=$oname;
			}
		}
	}
	if(!strlen($sval))
	{
		$sval='x_'.$sel;
		$cva['x_'.$sel]='?';
	}
//fxDebug($cva, $sval);

	return(array('cva'=>$cva, 'sval'=>$sval));
}

/**
 * ???FUNCTION???
 *
 * @param various $zids    - Mandatory parameter: ???PARAMETER???
 * @param various $mid     - Mandatory parameter: ???PARAMETER???
 * @param various $lart_id - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function deleteHourEntries($zids,$mid,$lart_id)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// SQLs definieren
	$query_zeit="DELETE FROM zeitdaten WHERE zeit_id IN (".$zids.") AND mandanten_id=".$mid;
	if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
		fxDebug($query_zeit, 'ZEIT:');

	$query_taetigkeit="DELETE FROM taetigkeiten WHERE zeit_id IN (".$zids.") AND mandanten_id=".$mid;
	if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
		fxDebug($query_taetigkeit, 'TAETIGKEIT:');

	$query_lart='';
	if($lart_id)
	{
		$query_lart="DELETE FROM kostenzuordnung WHERE kostenstelle_id=".$lart_id." AND id IN (".$zids.") AND mandanten_id=".$mid." AND kostenstellenart=".FXP_TE_TIMEREC." AND kostendefinition=903";
		if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
			fxDebug($query_lart, 'LEISTUNGSART:');
	}

	// Daten aus DB löschen
	$err=db_x($query_zeit);
	if(!$err)
		$err=db_x($query_taetigkeit);
	if(!$err && $query_lart)
		$err=db_x($query_lart);

	return $err;
}

/**
 * ???FUNCTION???
 *
 * @param various $doc_did - Mandatory parameter:               ???PARAMETER???
 * @param integer $doc_rid - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getDocInfo($doc_did, $doc_rid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$arr=array('doc_dbid'=>(int)$doc_did, 'doc_drid'=>(int)$doc_rid, 'doc_id'=>"", 'doc_path'=>"", 'doc_fs'=>"", 'doc_oname'=>"", 'doc_name'=>"", 'doc_dir'=>"", 'doc_file'=>"", 'doc_icon'=>$GLOBALS['gfxpath'].'ico_def.png', 'doc_type'=>"", 'doc_size'=>0, 'doc_ptyp'=>0, 'doc_prnt'=>0, 'doc_vers'=>0);
	if($arr['doc_dbid'])
	{
		if(!isset($GLOBALS['doc']))
			$GLOBALS['doc']=new fxpDoc();

		$sql="SELECT dok_id, dok_originalname, dok_art, elter_art, elter FROM d_dokumente WHERE dok_id=".$arr['doc_dbid']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$dta=db_values($sql);
//fxDebug($dta,$sql);
		if(fxIsArray($dta))
		{
			$arr['doc_oname']=$dta['dok_originalname'];
			$arr['doc_name']=$dta['dok_originalname'];
			$arr['doc_type']=$dta['dok_art'];
			$arr['doc_ptyp']=$dta['elter_art'];
			$arr['doc_prnt']=$dta['elter'];
		}

		if($arr['doc_drid'])
			$sqa='dok_verlauf_id='.$arr['doc_drid'];
		else
			$sqa='aktiv_kz=1';
		$sql="SELECT dok_verlauf_id, dok_verlauf_id, pfad, datei_name, dateigroesse, version_nr, aktiv_kz FROM d_dok_verlauf WHERE dok_id=".$arr['doc_dbid']." AND ".$sqa." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql);
//fxDebug($tmp,$sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $tk => $tv)
				$dta[$tk]=$tv;

			$arr['doc_drid']=(int)$tmp['dok_verlauf_id'];
			$arr['doc_name']=$tmp['datei_name'];
			$arr['doc_dir']=$GLOBALS['doc']->cpath.$tmp['pfad'];
			$arr['doc_file']=$tmp['pfad'];
			$arr['doc_size']=(int)$tmp['dateigroesse'];
			$arr['doc_vers']=$tmp['version_nr'];

			// File icon from suffix
			$sdp=strrpos($arr['doc_file'], '.');
			if($sdp !== false)
			{
				$suffix=strtolower(substr($arr['doc_file'],$sdp+1));
				if(file_exists($GLOBALS['gfxpath'].'ico_'.$suffix.'.png'))
					$arr['doc_icon']=$GLOBALS['gfxpath'].'ico_'.$suffix.'.png';
			}
		}
//fxDebug($dta,'$dta');
		$da=$GLOBALS['doc']->getDocPath($dta['elter_art'].'_'.$dta['elter'], $dta);
//fxDebug($da,'$da');
		$arr['doc_id']=$da['id'];
		$arr['doc_path']=$da['full'];
		$arr['doc_fs']=$GLOBALS['doc']->fs;
	}
//fxDebug($arr,'$arr');

	return $arr;
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function getDocIcons()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dt_icons=array
	(
		'zip'=>'zip', 'ace'=>'zip', 'rar'=>'zip',
		'doc'=>'doc',
		'txt'=>'txt',
		'bmp'=>'pic', 'jpg'=>'pic', 'gif'=>'pic', 'png'=>'pic', 'tif'=>'pic',
		'xls'=>'xls',
		'pdf'=>'pdf',
		'exe'=>'exe', 'com'=>'exe', 'bat'=>'exe',
		'eml'=>'eml', 'msg'=>'eml',
		'htm'=>'htm', 'php'=>'htm', 'inc'=>'htm',
		'xml'=>'xml',
		'mp3'=>'mp3', 'wav'=>'mp3',
		'hlp'=>'hlp', 'chm'=>'hlp',
		'mdb'=>'mdb',
		'?'=>'fxp'
	);

	return $dt_icons;
}

/**
 * ???FUNCTION???
 *
 * @param various $type - Mandatory parameter:               ???PARAMETER???
 * @param integer $id   - Optional parameter (default = 0):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getDocRights($type, $id=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$user=$GLOBALS['fxpglobals']['user'];
	$pers=$GLOBALS['fxpglobals']['person'];

	if(($type == 'id') && (!$pers || ($user == 1) || $GLOBALS['fxpglobals']['persdat']['roles'][2]))
		return 1;

	// General person or project filters
	if(!isset($GLOBALS['__da_prsfilter']))
		$GLOBALS['__da_prsfilter']="SELECT p.personen_id".filter_person('from', true, false, '', true);
	if(!isset($GLOBALS['__da_prjfilter']))
		$GLOBALS['__da_prjfilter']="SELECT p.projekt_id".filter_projekt('from', true, false, '', true);
//fxDebug($GLOBALS['__da_prsfilter'],'$GLOBALS[\'__da_prsfilter\']');
//fxDebug($GLOBALS['__da_prjfilter'],'$GLOBALS[\'__da_prjfilter\']');

	// Generate rights sql
	$sql=" AND d.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (";
	// ...Rights, because creator or last changer
	$sql .= $GLOBALS['nl']."(d.angelegt_von=".$user." OR d.aenderungs_id=".$pers.")";
	// ...Rights, because global (1)
	if(tr_rechte(67))
		$sql .= $GLOBALS['nl']." OR (d.elter_art=1)";
	// ...Rights, because backups (2) with assigned transaction (250)
	if(tr_rechte(250))
		$sql .= $GLOBALS['nl']." OR (d.elter_art=2)";
	// ...Rights, because emails (3), persons (6) or contracts (9) for all, self or assigned persons
	$sql .= $GLOBALS['nl']." OR ((d.elter_art=3 OR d.elter_art=6 OR d.elter_art=9) AND (d.elter<=0 OR d.elter=".$pers." OR d.elter IN (".$GLOBALS['__da_prsfilter'].")))";
	// ...Rights, because materials (4) with at least one assigned transaction (78,90,102,154,200)
	if(tr_rechte(78) || tr_rechte(90) || tr_rechte(102) || tr_rechte(154) || tr_rechte(200))
		$sql .= $GLOBALS['nl']." OR (d.elter_art=4)";
	// ...Rights, because milestones (5) for all or for assigned projects
	$sql .= $GLOBALS['nl']." OR (d.elter_art=5 AND (d.elter<=0 OR d.elter IN (SELECT meilenstein_id FROM meilensteine WHERE projekt_id IN (".$GLOBALS['__da_prjfilter']."))))";
	// ...Rights, because projects (7) for all or for assigned projects
	$sql .= $GLOBALS['nl']." OR (d.elter_art=7 AND (d.elter<=0 OR d.elter IN (".$GLOBALS['__da_prjfilter'].")))";
	// ...Rights, because invoices (8) for assigned persons and assigned transaction 105,117
	$riout=tr_rechte(105);	// Rights for "Outgoing Invoices" (2580)
	$riinc=tr_rechte(117);	// Rights for "Incoming Invoices" (3102)
	if($riout || $riinc)
	{
		$sql .= $GLOBALS['nl']." OR (d.elter_art=8 AND ((d.elter<=0 OR d.elter=".$pers.") OR (d.elter IN (".$GLOBALS['__da_prsfilter'].") AND (";
		if($riout)
		{
			$sql .= "(d.dok_art=2580 AND d.elter IN (SELECT kunde FROM rechnung WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']."))";
			if($riinc)
				$sql .= " OR ";
		}
		if($riinc)
			$sql .= "(d.dok_art=3102 AND d.elter IN (SELECT personen_id FROM rechnungseingang WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']."))";
		$sql .= "))))";
	}
	// ...Rights, because assigned user
	$sql .= $GLOBALS['nl']." OR d.dok_id IN (SELECT id_referenz FROM rechte_referenz WHERE benutzergr_id=".$user;
	$sql .= " AND gruppeart=2163 AND rechte_ref=38 AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
	// ...Rights, because in assigned user group
	if(!isset($GLOBALS['__da_ugp']))
	{
		$GLOBALS['__da_ugp']=get_benutzergruppen($user);
		if(is_array($GLOBALS['__da_ugp']))
			$GLOBALS['__dc_ugp']=array_to_csv($GLOBALS['__da_ugp']);
	}
	if(isset($GLOBALS['__dc_ugp']))
	{
		$sql .= $GLOBALS['nl']." OR d.dok_id IN (SELECT id_referenz FROM rechte_referenz WHERE benutzergr_id IN (".$GLOBALS['__dc_ugp'].")";
		$sql .= " AND gruppeart=2164 AND rechte_ref=38 AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
	}
	// ...Rights, because in assigned department
	if(!isset($GLOBALS['__da_dep']))
	{
		$GLOBALS['__da_dep']=get_abteilungen($pers);
		if(is_array($GLOBALS['__da_dep']))
			$GLOBALS['__dc_dep']=array_to_csv($GLOBALS['__da_dep']);
	}
	if(isset($GLOBALS['__dc_dep']))
	{
		$sql .= $GLOBALS['nl']." OR d.dok_id IN (SELECT id_referenz FROM rechte_referenz WHERE benutzergr_id IN (".$GLOBALS['__dc_dep'].")";
		$sql .= " AND gruppeart=2165 AND rechte_ref=38 AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
	}
	// ...Rights, because involved in assigned project
	if(strlen($GLOBALS['__da_prjfilter']))
	{
		$sql .= $GLOBALS['nl']." OR d.dok_id IN (SELECT id_referenz FROM rechte_referenz WHERE benutzergr_id IN (".$GLOBALS['__da_prjfilter'].")";
		$sql .= " AND gruppeart=1920 AND rechte_ref=38 AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
	}
	$sql .= $GLOBALS['nl'].")";

	if($type == 'sql')
		return $sql;

	$sql="SELECT COUNT(d.dok_id) FROM d_dokumente d WHERE d.dok_id=".(int)$id.$sql;
	$rights=(int)db_value($sql);
//fxDebug($rights,$sql);

	return $rights;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $once_per_day - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function createRepIncInvoices($once_per_day=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$esa=array('ec'=>0, 'sc'=>0);

	if($once_per_day)
	{
		$sql="SELECT zeitstempel FROM teinstellung WHERE trans_id=117 AND benutzer_id=0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$ldt=db_value($sql);
		if(strlen($ldt) && (substr($ldt,0,8) >= substr($GLOBALS['datetime'],0,8)))
			return $esa;

		if(strlen($ldt))
			$sql="UPDATE teinstellung SET beschreibung='CHK ".$GLOBALS['datetime']."' WHERE trans_id=117 AND benutzer_id=0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		else
			$sql="INSERT INTO teinstellung (mandanten_id, trans_id, benutzer_id, beschreibung) VALUES (".$GLOBALS['fxpglobals']['client'].", 117, 0, 'CHK ".$GLOBALS['datetime']."')";
		$err=db_x($sql);
//fxDebug($err,$sql);
	}

	// Get all running source repetition incoming invoices
	$sql  = "SELECT * FROM rechnungseingang WHERE wiederholung>0 AND wiederholungart>0";
	$sql .= " AND bezahltermin<='".substr($GLOBALS['datetime'],0,8)."240000' AND (endedatum_wdh IS NULL OR endedatum_wdh>='".substr($GLOBALS['datetime'],0,8)."240000')";
	$sql .= " AND (projekt_id=0 OR projekt_id IN (SELECT projekt_id FROM projekte WHERE projektart=".FXP_PRJ_TASK." AND projekt_status=".FXP_PS_ACTIVE." AND mandanten_id=".$GLOBALS['fxpglobals']['client']."))";
	$sql .= " AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$rsa=db_values($sql,'*');
//fxDebug($rsa,$sql);
	if(fxIsArray($rsa))
	{
		foreach($rsa as $sa)
		{
			$id=(int)$sa['rechnungs_id'];

			// Get all repetition incoming invoices for this source
			$rct=1;
			$rra=array();
			$sql="SELECT * FROM rechnungseingang WHERE wiederholungart=0 AND puffer=".$id." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY bezahltermin";
			$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
			if(fxIsArray($tmp))
			{
				foreach($tmp as $t)
				{
					$rra[substr($t['bezahltermin'],0,8)]=$t;
					if($t['wiederholung'] >= $rct)
						$rct=$t['wiederholung']+1;
				}
			}
//fxDebug($ra,'$ra: $rct='.$rct);

			// Create repetition cycle scheduler
			$ca=array();
			$sd=substr($sa['bezahltermin'],0,8);
			$ed=fx_date('Ymd', fx_mktime(12,0,0, (int)substr($GLOBALS['datetime'],4,2), (int)$sa['puffer']+(int)substr($GLOBALS['datetime'],6,2), (int)substr($GLOBALS['datetime'],0,4)));
//echo('$sd='.$sd.', $ed='.$ed.'<hr />');
			$ad=$sd;
			while($ad <= $ed)
			{
				if($ad > $sd)
				{
//echo('&rarr; $ad='.$ad.'<br />');
					if(!isset($rra[$ad]))
					{
						$ca[$ad]=$rct;
						$rct++;
					}
				}

				// Create next cycle date
				$ady=(int)substr($ad,0,4);
				$adm=(int)substr($ad,4,2);
				$add=(int)substr($ad,6,2);
				switch($sa['wiederholungart'])
				{
					case 666:	// Weekly
						$add += $sa['wiederholung']*7;
					break;

					case 667:	// Monthly
						$adm += $sa['wiederholung'];
					break;

					case 664:	// Quarterly
						$adm += $sa['wiederholung']*3;
					break;

					case 668:	// Yearly
						$ady += $sa['wiederholung'];
					break;

					default:	// Daily
						$add += $sa['wiederholung'];
					break;
				}
				$ad=fx_date('Ymd', fx_mktime(12,0,0, $adm,$add,$ady));
			}
//fxDebug($ca,'$ca');

			// Create new repetition incoming invoice
			if(sizeof($ca))
			{
				foreach($ca as $dt => $dr)
				{
					$nid=id_max('rechnungseingang','rechnungs_id');
					$db_inv_number=db_convert_string($sa['rechnn_f'].' (R-'.$dr.')');

					if(strlen($sa['kaufdatum']))
						$db_date_purch="'".$sa['kaufdatum']."'";
					else
						$db_date_purch="NULL";

					$sql  = "INSERT INTO rechnungseingang (";
					$sql .= "mandanten_id, rechnungs_id, erechnungstyp, rechnn_f, projekt_id, personen_id, ";
					$sql .= "pname, person_angelegt, anlage_dtm, beschreibung, ";
					$sql .= "kaufdatum, bezahltermin, bezahlt_am, budgettyp, nettobetrag, mwst_satz, mehrwertsteuer, bruttobetrag, ";
					$sql .= "wiederholung, wiederholungart, puffer, endedatum_wdh, dok_id) VALUES (";
					$sql .= $GLOBALS['fxpglobals']['client'].", ".$nid.", ".$sa['erechnungstyp'].", ".$db_inv_number.", ".$sa['projekt_id'].", ".$sa['personen_id'].", ";
					$sql .= db_convert_string($sa['pname']).", ".(int)$sa['person_angelegt'].", '".$GLOBALS['datetime']."', ".db_convert_string($sa['beschreibung']).", ";
					$sql .= $db_date_purch.", '".$dt."000000', NULL, ".$sa['budgettyp'].", ".$sa['nettobetrag'].", ".$sa['mwst_satz'].", ".$sa['mehrwertsteuer'].", ".$sa['bruttobetrag'].", ";
					$sql .= $dr.", 0, ".$id.", NULL, 0)";

					$err=db_x($sql);
//fxDebug($err,$sql);
					if($err)
						$esa['ec']++;
					else
						$esa['sc']++;
				}
			}
		}
	}

	return $esa;
}

/**
 * ???FUNCTION???
 *
 * @param various $app - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getAcceptedApplicationID($app)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$app=(int)$app;
	$appca=array(
		FXP_AT_VACATIONREQUEST			=> FXP_AT_VACATION,
		FXP_AT_SPECIALVACATIONREQUEST	=> FXP_AT_SPECIALVACATION,
		FXP_AT_SICKCALL			=> FXP_AT_SICK,
		FXP_AT_TRAININGREQUEST		=> FXP_AT_TRAINING,
		FXP_AT_SLIDINGDAYREQUEST			=> FXP_AT_SLIDINGDAY,
		FXP_AT_OVERTIMEOUTPAYREQUEST	=> FXP_AT_OVERTIMEOUTPAY
	);

	if(isset($appca[$app]))
		return $appca[$app];
	return $app;
}

/**
 * ???FUNCTION???
 *
 * @param various $in_format   - Mandatory parameter:                 ???PARAMETER???
 * @param various $dat         - Mandatory parameter:                 ???PARAMETER???
 * @param various $cust_number - Mandatory parameter:                 ???PARAMETER???
 * @param various $cust_name   - Mandatory parameter:                 ???PARAMETER???
 * @param string  $mode        - Optional parameter (default = 'i'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getUniqueNumber($in_format, $dat, $cust_number, $cust_name, $mode='i')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=$GLOBALS['fxpglobals']['client'];
	if($GLOBALS['Mandanten_ID'])
		$mid=(int)$GLOBALS['Mandanten_ID'];

	$mode=strtolower(substr(trim($mode),0,1));

	// Formatierte eindeutige Rechnunssnummer anhand der Schablone erzeugen, z.B. FF-YY-nnn
	// ...Monat (M)
	$Monat=substr($dat, 4, 2); 
	$M_len=substr_count($in_format, 'M');
	$M_von=str_repeat('M', $M_len);
	$M_in=substr($Monat, 2-$M_len, $M_len);
//echo('<b>Monat:</b> $Monat='.$Monat.', $M_len='.$M_len.', $M_von='.$M_von.', $M_in='.$M_in.'<BR>');

	// ...Jahr (Y)
	$Jahr=substr($dat, 0, 4);
	$J_len=substr_count($in_format, 'Y');
	$J_von=str_repeat('Y', $J_len);
	$J_in=substr($Jahr, 4-$J_len, $J_len);
//echo('<b>Jahr:</b> $anlage_dtm='.$anlage_dtm.', $Jahr='.$Jahr.', $J_len='.$J_len.', $J_von='.$J_von.', $J_in='.$J_in.'<BR>');

	// ...Firmenname (F)
	$F_len=substr_count($in_format, 'F');
	$F_von=str_repeat('F', $F_len);
	$F_in=substr($cust_number, 0, $F_len);
	if(!strlen($F_in))	// Keine Kundennummer, dann Kundenname wählen
		$F_in=substr($cust_name, 0, $F_len);

	// ...Nummer (n)
	$N_len=substr_count($in_format, 'n');
	$N_pos=strpos($in_format, 'n');
	$N_von=str_repeat('n', $N_len);
	$N_in=str_repeat('_', $N_len);
	$suchen=array("'$N_von'si", "'$F_von'si", "'$J_von'si", "'$M_von'si");
	$ersetzen=array($N_in, $F_in, $J_in, $M_in);
	$RN_Form=preg_replace($suchen, $ersetzen, $in_format);

	// Höchste bereits vorhandene Nummer ermitteln, welche diesem Format entspricht
	$sql="SELECT rechnn_f FROM rechnung WHERE mandanten_id=".$mid." AND ".db_like('rechnn_f', $RN_Form)." ORDER BY rechnn_f DESC";
	$N_fnd=db_value($sql);
//fxDebug($N_fnd, $sql);
	if(strlen($N_fnd))
		$Fort_KN=1+(int)substr($N_fnd, $N_pos, $N_len);
	else
		$Fort_KN=1;
//fxDebug($Fort_KN, '$Fort_KN');
	$N_in=str_pad($Fort_KN, $N_len, '0', STR_PAD_LEFT);

	// Auf Eindeutigkeit der Nummer prüfen
	$maxtries=100;
	$FormTry='';
	do
	{
		$suchen=array("'$N_von'si", "'$F_von'si", "'$J_von'si", "'$M_von'si");
		$ersetzen=array($N_in, $F_in, $J_in, $M_in);
		$RN_Form=preg_replace($suchen, $ersetzen, $in_format);
		if(!strlen($FormTry))
			$FormTry=$RN_Form;

		$RN_exsql="SELECT rechnungs_id FROM rechnung WHERE mandanten_id=".$mid." AND ".db_like('rechnn_f', $RN_Form);
		$RN_exists=db_value($RN_exsql);
//fxDebug($RN_exists, $RN_exsql);
		if($RN_exists)
		{
			$Fort_KN++;
			$N_in=str_pad($Fort_KN, $N_len, '0', STR_PAD_LEFT);
			$maxtries--;
			if($maxtries < 1)
			{
				$err=true;
				if($mode == 'i')
				{
					// Alle Versuche eine eindeutige Rechnungsnummer [z.B. $var1] zu generieren schlugen fehl! Bitte überprüfen Sie das Nummernformat [$var2] in den Einstellungen oder legen Sie für den Auftraggeber eine eindeutige Identifikationsnummer fest.
					error_msg(313, false, '<b>'.$FormTry.'</b>', '<b>'.$in_format.'</b>');
				}
				return '';
			}
		}
	} while($RN_exists);

	return $RN_Form;
}

/**
 * ???FUNCTION???
 *
 * @param various $rchg_id - Mandatory parameter: ???PARAMETER???
 */
function get_invoiced_positions($rchg_id)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$rchg_id=(int)$rchg_id;
	$GLOBALS['ca_ip']=array();
	$mid=$GLOBALS['fxpglobals']['client'];
	if($GLOBALS['Mandanten_ID'])
		$mid=(int)$GLOBALS['Mandanten_ID'];

	$rasql  = "SELECT ist_sachmittel, projekt_id, artikelzuord_id, personen_id, buendelpos,";
	$rasql .= " vk_projekt, r_anzahl, verkaufseinheit_id, mwst_satz, gesamtpreis, mwst, r_gesamtbrutto,";
	$rasql .= " allg_textfeld_a, anmerkung, r_bezeichnung, p_name";
	$rasql .= " FROM rechnungsposition";
	$rasql .= " WHERE rechnungs_id=".$rchg_id." AND projekt_id>-2 AND mandanten_id=".$mid;
	$rasql .= " ORDER BY positions_id";
	$rainfo=db_values($rasql, '*');
//fxDebug($rainfo, $rasql);
	if(fxIsArray($rainfo))
	{
		foreach($rainfo as $rai)
		{
			$ra_typ=(int)$rai['ist_sachmittel'];
			$ra_pid=(int)$rai['projekt_id'];
			$ra_aid=(int)$rai['artikelzuord_id'];
			$ra_rid=(int)$rai['personen_id'];
			$ra_bnd=(int)$rai['buendelpos'];
			$ra_epn=round($rai['vk_projekt'], 2);
			$ra_anz=round($rai['r_anzahl'], 2);
			$ra_gpn=round($rai['gesamtpreis'], 2);
			$ra_gpm=round($rai['mwst'], 2);
			$ra_gpb=round($rai['r_gesamtbrutto'], 2);
			$ra_anm=$rai['anmerkung'];
			$ra_bez=$rai['allg_textfeld_a'];
			if(!strlen($ra_bez))
				$ra_bez=$rai['r_bezeichnung'];

			// Bündelpos
			if($ra_pid == -1)
			{
				$ra_epn=0.00;
				if($ra_anz > 0.00)
					$ra_epn=round($ra_gpn/$ra_anz, 2);
				$GLOBALS['ca_ip'][$ra_typ.'_'.$ra_aid]=array('rid'=>$rchg_id, 'epn'=>$ra_epn, 'anz'=>$ra_anz, 'ein'=>(int)$rai['verkaufseinheit_id'], 'gpn'=>$ra_gpn, 'mws'=>round($rai['mwst_satz'], 2), 'gpm'=>$ra_gpm, 'gpb'=>$ra_gpb, 'bez'=>$ra_bez, 'anm'=>$ra_anm);
			}
			// Rechnungsposition
			else
			{
				$GLOBALS['mit_abgerechnet']=1;
				$GLOBALS['freigabe_ignorieren']=1;

				$pk=$ra_typ.'_'.$ra_pid;
				if($ra_typ == 50)		// P-FP - Festpreis (Personen)
					$GLOBALS['mit_PFP']=1;
				else if($ra_typ == 51)	// P-SB - Stundenbasis (Personen)
				{
					$GLOBALS['mit_PSB']=1;
					$pk .= '_'.$ra_rid;
				}
				else if($ra_typ == 52)	// RK - Reisekosten
				{
					$GLOBALS['mit_RK']=1;
					$pk .= '_'.$ra_rid.'_'.$ra_aid;

					$sql="SELECT rk_kategorie FROM zeitdaten WHERE zeit_id=".$ra_aid." AND mandanten_id=".$mid;
					$kat=(int)db_value($sql);
//fxDebug($kat, $sql);
					if($kat == 1980)
						$GLOBALS['mit_TA']=1;
				}
				else if($ra_typ == 53)	// M-SO - Sofort (Material)
				{
					$GLOBALS['mit_MSM']=1;
					$pk .= '_'.$ra_aid;
				}
				else if($ra_typ == 54)	// M-FP - Festpreis (Material)
					$GLOBALS['mit_MFP']=1;
				else if($ra_typ == 55)	// M-ZE - Zeit (Material)
				{
					$GLOBALS['mit_MZM']=1;
					$pk .= '_'.$ra_aid;
				}
				else if($ra_typ == 56)	// M-VB - Verbrauch (Material)
				{
					$GLOBALS['mit_MVM']=1;
					$pk .= '_'.$ra_aid;
				}

				if($ra_gpn == 0.00)
					$GLOBALS['leer_anzeigen']=1;

				$GLOBALS['ca_ip'][$pk]=array('rid'=>$rchg_id, 'bnd'=>$ra_bnd, 'epn'=>$ra_epn, 'anz'=>$ra_anz, 'ein'=>(int)$rai['verkaufseinheit_id'], 'gpn'=>$ra_gpn, 'mws'=>round($rai['mwst_satz'], 2), 'gpm'=>$ra_gpm, 'gpb'=>$ra_gpb, 'bez'=>$ra_bez, 'anm'=>$ra_anm);
			}
		}
	}
//fxDebug($GLOBALS['ca_ip'], 'ca_ip');
}

/**
 * ???FUNCTION???
 *
 * @param various $ra            - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $with_document - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_invoice_number($ra, $with_document=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//fxDebug($ra,'$ra');

	if(!isset($GLOBALS['sta']) || !is_array($GLOBALS['sta']) || !sizeof($GLOBALS['sta']))
	{
		$GLOBALS['sta']=fxf_getReferenceValues(2019);		// 3090=>Open, 3091=>Paid
		$GLOBALS['sta'][1644]=ucfirst(meldung(1644,false));	// 1644=>Created
		$GLOBALS['sta'][2271]=ucfirst(meldung(2271,false));	// 2271=>Canceled
		$GLOBALS['sta'][2746]=ucfirst(meldung(2746,false));	// 2746=>Abhängig
		$GLOBALS['sta'][3028]=meldung(3028,false);			// 3028=>Komplett (= Schlußrechnung)
//fxDebug($GLOBALS['sta'],'$sta');
	}

	$icchg=true;
	$icdel=true;

	// Color + Status
	$style='';
	if($ra['archiv'])				// Canceled
	{
		$status_text=$GLOBALS['sta'][2271];
		$icchg=false;
		if($ra['ausgegangen_am'])
			$icdel=false;
		$color='grey strike';
		$icon=$GLOBALS['gfxpath'].'tl_grey.png';
		$status='<img src="'.$icon.'" align="bottom">&nbsp;<i class="darkgrey">'.$status_text.'</i>';
	}
	else if($ra['bezahlt_am'])		// Paid
	{
		$status_text=$GLOBALS['sta'][3091];
		$icchg=false;
		$icdel=false;
		$color='green';
		$icon=$GLOBALS['gfxpath'].'tl_green.png';
		if($ra['bezahlt'] != $ra['bruttobetrag'])
		{
			$bc='e1001a';
			if($ra['bezahlt'] > $ra['bruttobetrag'])
				$bc='009f6b';
			$style='border-radius:6px;box-shadow:0 0 6px #'.$bc.';';
		}
		$status='<img src="'.$icon.'" align="bottom" style="'.$style.'">&nbsp;<i class="'.$color.'">'.$status_text.'</i>';
	}
	else if($ra['ausgegangen_am'])	// Open
	{
		if(!isset($GLOBALS['invoptt']))
			$GLOBALS['invoptt']=explode('|', meldung(3284,array()));	// The due date (= Expected receipt of payment) of the invoice$var1 has <b>not</b> yet expired - there are still $var2 day(s) left.|The due date (= Expected receipt of payment) of the invoice$var1 is <b>Today</b>!|The due date (= Expected receipt of payment) of the invoice$var1 is already $var2 day(s) <b>exceeded</b>!
//fxDebug($GLOBALS['invoptt'],'$GLOBALS[\'invoptt\']', 0);

		$status_text=$GLOBALS['sta'][3090];
		$icchg=false;

		$rdate=substr($ra['ausgegangen_am'],0,8);
		$tdate=substr($GLOBALS['datetime'],0,8);
		if(isset($ra['stichtag']) && strlen($ra['stichtag']))
		{
			$ddate=substr($ra['stichtag'],0,8);
			$fddate=' (= <b>'.get_entry($ddate,'datum').'</b>)<br />';
		}
		else
		{
			$ddate=substr(time_add($rdate,14,'d'),0,8);
			$fddate=' ('.get_entry($rdate,'datum').' &rarr; <b>'.get_entry($ddate,'datum').'</b>)<br />';
		}

		if($ddate >= $tdate)
		{
			$color='black';
			$icon=$GLOBALS['gfxpath'].'tl_yellow.png';
			// The due date (= Expected receipt of payment) of the invoice$var1 has <b>not</b> yet expired - there are still $var2 day(s) left.
			if($ddate > $tdate)
				$tooltip='<i>'.strtr($GLOBALS['invoptt'][0], array('$var1'=>$fddate, '$var2'=>'<b>'.fxp_date_diff($tdate,$ddate).'</b>')).'</i>';
			// The due date (= Expected receipt of payment) of the invoice$var1 is <b>Today</b>!
			else
				$tooltip='<i>'.strtr($GLOBALS['invoptt'][1], array('$var1'=>$fddate)).'</i>';
		}
		else
		{
			$color='red';
			$icon=$GLOBALS['gfxpath'].'tl_red.png';
			// The due date (= Expected receipt of payment) of the invoice$var1 is already $var2 day(s) <b>exceeded</b>!
			$tooltip='<i class='.$color.'>'.strtr($GLOBALS['invoptt'][2], array('$var1'=>$fddate, '$var2'=>fxp_date_diff($ddate,$tdate))).'</i>';
		}
//		$tooltip .= '<hr />$rdate='.$rdate.'<br />$tdate='.$tdate.'<br />$ddate='.$ddate;
		$status='<span tooltip="'.$tooltip.'" style="cursor:help;"><img src="'.$icon.'" align="bottom">&nbsp;<i class="'.$color.'">'.$status_text.'</i></span>';
	}
	else							// Created
	{
		$status_text=$GLOBALS['sta'][1644];
		$color='lightgrey';
		$icon=$GLOBALS['gfxpath'].'tl_neutral.png';
		$status='<img src="'.$icon.'" align="bottom">&nbsp;<i class="'.$color.'">'.$status_text.'</i>';
	}

	// Completion invoice
	if($ra['kalktyp'] == 2734)
	{
		$icchg=false;
		$status_text .= ' / '.$GLOBALS['sta'][3028];
		$status .= '&nbsp;&nbsp;<b class="grey" tooltip="'.$GLOBALS['sta'][3028].'" style="cursor:help;border:1px solid #888888;border-radius:3px;padding:2px;">'.substr($GLOBALS['sta'][3028],0,1).'</b>';
	}

	// Check if invoice is already linked in other invoices (Completion invoice)
	$dependant='';
	$sql  = "SELECT p.rechnungs_id, r.rechnn_f FROM rechnungsposition p, rechnung r";
	$sql .= " WHERE p.projekt_id=-2 AND p.verkaufseinheit_id=".$ra['rechnungs_id']." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND r.rechnungs_id=p.rechnungs_id AND r.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$air=db_values($sql);
//fxDebug($air,$sql);
	if(fxIsArray($air))
	{
		$icchg=false;
		$icdel=false;
		$dependant=$GLOBALS['sta'][2746].'&nbsp;<i>(&rarr;&nbsp;'.$air['rechnn_f'].')</i>';
		$status_text .= ' / '.$dependant;
		$status .= '&nbsp;&nbsp;<b class="grey" tooltip="'.$dependant.'" style="cursor:help;font-size:13pt;">&rarr;</b>';
	}

	$inv_number=$ra['rechnn_f'];

	// PDF-Dokument
	$doc_id=0;
	$doc_name='';
	$doc_icon='';
	if($with_document && (tr_rechte(107) || tr_rechte(110)))	// Recht auf 107="Dokumente suchen" oder 110="Dokumentenmanager"
	{
		$sql  = "SELECT dok_id FROM d_dokumente";
		$sql .= " WHERE elter_art=8 AND dok_art=2580 AND id=".$ra['rechnungs_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$doc_id=(int)db_value($sql);
//fxDebug($doc_id, $sql);

		if($doc_id || $ra['archiv'] || $ra['ausgegangen_am'])
		{
			if($ra['archiv'])
				$doc_date=substr($ra['archiv'],0,8);
			else if($ra['ausgegangen_am'])
				$doc_date=substr($ra['ausgegangen_am'],0,8);

			if(!isset($GLOBALS['rw_rechnung']))
				$GLOBALS['rw_rechnung']=meldung(872,false);	// Rechnung

			if($doc_id)
			{
				$darr=getDocInfo($doc_id);
//fxDebug($darr,'$darr: $doc_id='.$doc_id);
				$doc_name=$darr['doc_name'];
				// Check file existence
				if(file_exists($darr['doc_dir']))
				{
					// Check access right
					$right=getDocRights('id',$doc_id);
					if($right)
					{
						if(!isset($GLOBALS['tt_path']))
							$GLOBALS['tt_path']=meldung(1160,false);	// Path
						if(!isset($GLOBALS['tt_size']))
							$GLOBALS['tt_size']=get_text(1094,'10');	// Size

						$apt=fxReplaceSpecialCharacters($darr['doc_path'].$darr['doc_fs'].$darr['doc_file'],false);
						$afn=fxReplaceSpecialCharacters($darr['doc_name'],false);
						$asz=$darr['doc_size'];
						$aid=$darr['doc_id'].'.'.$darr['doc_dbid'].'.'.$darr['doc_drid'];
						$adt=$GLOBALS['doc']->getDocType($afn);
						$aca=getMIMETypes($adt,true);

						// Displayable?
						$ata='';
						if(!$aca['display'])
							$ata=$GLOBALS['ddtd'];	// If the browser cannot display the document directly it will be offered to download and/or to display with a different program
						$att=' tooltip="<b>'.$GLOBALS['ddt'][0].'</b><br /><br /><table><tr><td class=grey>'.$GLOBALS['tt_path'].':&nbsp;</td><td>'.$darr['doc_path'].'</td></tr><tr><td class=grey>'.$GLOBALS['tt_size'].':&nbsp;</td><td>'.convBytes($darr['doc_size'], 'KB').'</td></tr></table>'.$ata.'"';	// Display document in a new window
						$aoc=' '.fxf_jsFunction('dmsAction', 'dsw', 0, 'spath='.$apt.'&seid='.$aid.'&sname='.$afn);
						$inv_number='<u class="'.$color.' bold">'.$inv_number.'</u>';
						$doc_icon='<div'.$att.$aoc.' style="cursor:pointer;"><img src="'.$darr['doc_icon'].'" align="top">&nbsp;'.$inv_number.'</div>';
					}
					else	// You don't have the necessary access rights for this document
					{
						$inv_number='<i class="'.$color.' bold">'.$inv_number.'</i>';
						$doc_icon='<div tooltip="'.$GLOBALS['ddt'][2].'" style="font-style:italic;opacity:0.70;cursor:help;"><img src="'.$darr['doc_icon'].'" align="top">&nbsp;'.$inv_number.'</div>';
					}
				}
				else	// Document doesn't exist anymore
				{
					$inv_number='<i class="'.$color.'">'.$inv_number.'</i>';
					$doc_icon='<div tooltip="'.$GLOBALS['ddt'][1].'" style="font-style:italic;opacity:0.50;cursor:help;"><img src="'.$darr['doc_icon'].'" align="top">&nbsp;'.$inv_number.'</div>';
				}
			}
			else	// No document assigned yet!
			{
				$inv_number='<span class="'.$color.'">'.$inv_number.'</span>';
				$doc_icon='<div tooltip="'.$GLOBALS['ddt'][4].'" style="font-style:italic;opacity:0.90;cursor:help;"><img src="'.$GLOBALS['gfxpath'].'0.png" align="top" style="margin:5px 4px;margin-bottom:0;">&nbsp;'.$inv_number.'</div>';
			}
		}
		else	// This document has not been created yet!
		{
			$inv_number='<span class="'.$color.'">'.$inv_number.'</span>';
			$doc_icon='<div tooltip="'.$GLOBALS['ddt'][5].'" style="font-style:italic;opacity:0.90;cursor:help;"><img src="'.$GLOBALS['gfxpath'].'0.png" align="top" style="margin:5px 4px;margin-bottom:0;">&nbsp;'.$inv_number.'</div>';
		}
	}

	if(strlen($doc_icon))
		$inv_number=$doc_icon;
	else
		$inv_number='<b class="'.$color.'" tooltip="'.$status_text.'" style="cursor:help;">'.$inv_number.'</b>';

	$ra=array('color'=>$color, 'status_text'=>$status_text, 'inv_number'=>$inv_number, 'icon'=>$icon, 'status'=>$status, 'style'=>$style, 'dependent'=>$dependent, 'doc_id'=>$doc_id, 'doc_name'=>$doc_name, 'doc_icon'=>$doc_icon, 'icchg'=>$icchg, 'icdel'=>$icdel);
//fxDebug($ra,'$ra');

	return $ra;
}

/**
 * ???FUNCTION???
 *
 * @param various $mid  - Mandatory parameter:                 ???PARAMETER???
 * @param double  $act  - Optional parameter (default = 0.0):  ???PARAMETER???
 * @param double  $ent  - Optional parameter (default = 0.0):  ???PARAMETER???
 * @param string  $text - Optional parameter (default = ''):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function adjustMaterialStock($mid, $act=0.0, $ent=0.0, $text='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Error code and success counter variables
	$ec=0;
	$sc=0;

	if($mid)
	{
		// Check stock
		$sid=0;
		$sact=0.0;
		$suse=0.0;
		$sent=0.0;
		$srem=0.0;
		$sql="SELECT bestand_id, bestand_akt, bestand_verbr, bestand_zug, bestand_abg FROM materialbestand WHERE sachmittel_id=".$mid." AND bestand_akt IS NOT NULL AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sma=db_values($sql);
//fxDebug($sma,$sql);
		if(fxIsArray($sma))
		{
			$sid=(int)$sma['bestand_id'];
			$sact=(float)$sma['bestand_akt'];
			$suse=(float)$sma['bestand_verbr'];
			$sent=(float)$sma['bestand_zug'];
			$srem=(float)$sma['bestand_abg'];
		}

		$sql="SELECT bestand_id FROM materialbestand WHERE sachmittel_id=".$mid." AND bestand_akt IS NULL AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$eid=(int)db_value($sql);
//fxDebug($eid,$sql);

		// Check material
		$sql="SELECT lagerartikel, verwendungsart FROM sachmittel WHERE sachmittel_id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$mta=db_values($sql);
//fxDebug($mta,$sql);

		// Is the material a stocked usage material?
		$sum=false;
		if(fxIsArray($mta) && $mta['lagerartikel'] && ($mta['verwendungsart'] == 3150))
			$sum=true;
//fxDebug($sum,'$sum (Stocked usage material)');

		// Adjust material stock?
		if(fxIsArray($mta) && $sum)
		{
			// Entered
			$sql="SELECT SUM(bestand_zug) AS matent FROM materialbestand WHERE sachmittel_id=".$mid." AND bestand_akt IS NULL AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$aent=get_double(db_value($sql));
			if(!$aent)
				$aent=0.0;
//fxDebug($aent,$sql);

			// Removed
			$sql="SELECT SUM(bestand_abg) AS matrem FROM materialbestand WHERE sachmittel_id=".$mid." AND bestand_akt IS NULL AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$arem=get_double(db_value($sql));
			if(!$arem)
				$arem=0.0;
//fxDebug($arem,$sql);

			$asum=$aent-$arem;

			// Used
			$sql="SELECT SUM(sachmenge) AS matuse FROM materialverbrauch WHERE sachmittel_id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$nuse=get_double(db_value($sql));
			if(!$nuse)
				$nuse=0.0;
//fxDebug($nuse,$sql);

			$nent=$aent;
			$nrem=$arem;
			if($ent > 0.0)
			{
				$nent=$aent+$ent;
				$nact=$nent-$nrem;
			}
			else if($ent < 0.0)
			{
				$nrem=$arem-$ent;
				$nact=$nent-$nrem;
			}
			else if($act || ($text == '(*def*)'))
			{
				$nact=$act;
				if($nact > $sact)
					$nent=$aent+$nact-$sact;
				else if($nact < $sact)
					$nrem=$arem+$sact-$nact;
				if($text == '(*def*)')
					$text='';
			}
			else
				$nact=$nent-$nrem;
//echo('$mid='.$mid.', $act='.$act.', $ent='.$ent.' - $sid='.$sid.', $eid='.$eid.'<br />$sact='.$sact.', $nact='.$nact.'<br />$suse='.$suse.', $nuse='.$nuse.'<br />$sent='.$sent.', $aent='.$aent.', $nent='.$nent.'<br />$srem='.$srem.', $arem='.$arem.', $nrem='.$nrem.'<br />$asum='.$asum.'<hr />');

			// More removed than is in stock?
			if($nact < 0.0)
				$ec=3174;	// There has been a greater material usage entered$var1 as there is actually in stock$var2!

			// Create stock sum set?
			else if(!$sid)
			{
				$sid=id_max('materialbestand','bestand_id');

				$sql  = "INSERT INTO materialbestand";
				$sql .= " (mandanten_id, bestand_id, sachmittel_id, bestand_akt, bestand_verbr, bestand_zug, bestand_abg, textzeile) ";
				$sql .= "VALUES";
				$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$sid.", ".$mid.", ".$nact.", ".$nuse.", ".$nent.", ".$nrem.", '(summary record)')";
				$err=db_x($sql);
//fxDebug($err,$sql);
				if($err)
					$ec=true;
				else
					$sc++;
			}

			// Adjust stock sum set?
			else
			{
				$sql="";
				if($nact != $sact)
					$sql .= ", bestand_akt=".$nact;
				if($nuse != $suse)
					$sql .= ", bestand_verbr=".$nuse;
				if($nent != $sent)
					$sql .= ", bestand_zug=".$nent;
				if($nrem != $srem)
					$sql .= ", bestand_abg=".$nrem;
				if(strlen($sql))
				{
					$sql="UPDATE materialbestand SET ".substr($sql,2)." WHERE bestand_id=".$sid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
//fxDebug($err,$sql);
					if($err)
						$ec=true;
					else
						$sc++;
				}
			}

			// Insert new entry or removal set
			if(!$ec && (($nent != $aent) || ($nrem != $arem)))
			{
				$dent=($nent-$aent);
				$drem=($nrem-$arem);

				$dtext=db_convert_string($text);

				$eid=id_max('materialbestand','bestand_id');

				$sql  = "INSERT INTO materialbestand";
				$sql .= " (mandanten_id, bestand_id, sachmittel_id, bestand_akt, bestand_verbr, bestand_zug, bestand_abg, textzeile) ";
				$sql .= "VALUES";
				$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$eid.", ".$mid.", NULL, NULL, ".$dent.", ".$drem.", ".$dtext.")";
				$err=db_x($sql);
//fxDebug($err,$sql);
				if($err)
					$ec=true;
				else
					$sc++;
			}
		}

		// Delete item from material stock?
		else if($sid || $eid)
		{
			$sql="DELETE FROM materialbestand WHERE sachmittel_id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$err=db_x($sql);
//fxDebug($err,$sql);
			if($err)
				$ec=3159;	// Error deleting the stockpile!
			else
				$sc++;
		}
	}

	if($ec === true)
		$ec=3158;	// Error adjusting the stockpile!

	return array('ec'=>$ec, 'sc'=>$sc);
}

/**
 * ???FUNCTION???
 *
 * @param various $eid - Mandatory parameter: ???PARAMETER???
 * @param various $sid - Mandatory parameter: ???PARAMETER???
 * @param various $stp - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function adjustCostServiceType($eid, $sid, $stp)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Error code and success counter variables
	$ec=0;
	$sc=0;

	$eid=(int)$eid;
	$sid=(int)$sid;
	$stp=(int)$stp;

	$osql="SELECT kostenstelle_id FROM kostenzuordnung WHERE id=".$eid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND kostenstellenart=".$stp." AND kostendefinition=903";
	$oid=(int)db_value($osql);
//fxDebug($oid,$osql);

	$sql='';
	if($sid)
	{
		$vf='00000000000000';
		$vu='99999999999999';
		if(!$oid)				// Create new assignment
		{
			$sql  = "INSERT INTO kostenzuordnung";
			$sql .= " (mandanten_id, kostenstelle_id, id, kostenstellenart, kostendefinition, gueltig_von, gueltig_bis) ";
			$sql .= "VALUES";
			$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$sid.", ".$eid.", ".$stp.", 903, '".$vf."', '".$vu."')";
		}
		else if($oid != $sid)	// Change assignment
		{
			$sql  = "UPDATE kostenzuordnung SET";
			$sql .= " kostenstelle_id=".$sid.", gueltig_von='".$vf."', gueltig_bis='".$vu."'";
			$sql .= " WHERE id=".$eid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND kostenstellenart=".$stp." AND kostendefinition=903";
		}
	}
	else if($oid)				// Delete assignment
	{
		$sql  = "DELETE FROM kostenzuordnung";
		$sql .= " WHERE kostenstelle_id=".$oid." AND id=".$eid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND kostenstellenart=".$stp." AND kostendefinition=903";
	}

	if(strlen($sql))
	{
		$err=db_x($sql);
//fxDebug($err,$sql);
		if($err)
			$ec=3208;	// Error adjusting the service type assignment!
		else
			$sc++;
	}

	return array('ec'=>$ec, 'sc'=>$sc);
}
?>