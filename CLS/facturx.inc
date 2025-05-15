<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : facturx.inc                                                  //
// Version     : 24.1                                                         //
// Begin       : 2025-01-16                                                   //
// Last Change : 2025-02-20                                                   //
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
 * Class for analyzing and creating Factur-X, i.e. electronic invoices
 * - XML name of the attachment in PDF/A must be: "factur-x.xml" or "xrechnung.xml"
 * - Embedded pictures must have format: CCITT Group 3 or 4, JBIG2, JPEG or JPEG2000
 * - Factur-X logo of the corresponding type should be embedded in PDF
 *
 * Validity check: https://www.elster.de/eportal/e-rechnung
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */
class FACTURX
{
	// -----------------------------------------------------------------
	// VARIABLES														
	// =================================================================

	// ...Protected
	protected $_version			= '1.0';		// Version number of this class
	protected $_debugmode		= 0;			// Bit 0=1: Funktionsaufruf
	protected $_tra				= array('"'=>'&quot;', "'"=>'&apos;', '<'=>'&lt;', '>'=>'&gt;', '&'=>'&amp;');

	// ...Public
	public $_error				= '';			// Error string
	public $_cid				= 0;			// Client id
	public $_lid				= 0;			// Language id
	public $_iid				= 0;			// Invoice id
	public $_languages			= array();		// Language array
	public $_fxa				= array();		// Factur-X array


	// -----------------------------------------------------------------
	// METHODS															
	// =================================================================

	/**
	 * ...Class constructor
	 */
	public function __construct($iid=0, $cid=0, $lid=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		// Error string
		$this->_error='';

		// Client id
		$cid=(int)$cid;
		if($cid > 0)
			$GLOBALS['fxpglobals']['client']=$cid;
		$this->_cid=$GLOBALS['fxpglobals']['client'];

		// Language id
		$lid=(int)$lid;
		if($lid > 0)
			$GLOBALS['fxpglobals']['lang']=$lid;
		$this->_lid=$GLOBALS['fxpglobals']['lang'];

		// Languages
		$la=fxGetAllLanguages();
		foreach($la as $lang => $a)
		{
			$act_lang=false;
			if($lang == $this->_lid)
				$act_lang=true;

			$this->_languages[$lang]=$act_lang;
		}

		// Invoice id
		$iid=(int)$iid;
		if($iid > 0)
			$err=$this->getInvoiceData($iid);
	}

	/**
	 * ...Class desturctor
	 */
	public function __destruct()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		foreach(array_keys(get_object_vars($this)) as $val)
		{
			if(isset($this->$val))
				unset($this->$val);
		}
	}

	/**
	 * ...version
	 *
	 * @param boolean $display - Optional parameter (default = false): Return version number (= false) or display it
	 *
	 * @return Class version number
	 */
	public function version($display=false)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();
		if($display)
			echo('fxp class: <b>FACTURX</b> -- version: '.$this->_version.'<br>');
		else
			return $this->_version;
	}

	/**
	 * ...setDebugMode
	 *
	 * @param integer $debugmode - Optional parameter (default = 0): Set debug mode to this value
	 */
	public function setDebugMode($debugmode=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->_debugmode=(int)$debugmode;
	}

	/**
	 * ...debugFunction
	 */
	protected function debugFunction()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if(!($this->_debugmode&1))
			return;

		$trace=debug_backtrace();
		extract($trace[1]);
		echo('<i><font color=aaaabb>PROJECT class:</font></i>&nbsp;&nbsp;&nbsp;<b><font color=blue>'.$function.'</font></b><font color=#bbbbcc>(</font>');
		if(isset($args) && is_array($args) && sizeof($args))
		{
			foreach($args as $cnt => $val)
			{
				if($cnt)
					echo('<font color=#bbbbcc>,</font> ');
				if(is_bool($val))
				{
					if($val)
						echo('<font color=00aa11>true</font>');
					else
						echo('<font color=aa1100>false</font>');
				}
				else if(is_null($val) || !strlen((string)$val))
					echo('<font color=#ffbbbb>~</font>');
				else if(is_string($val))
					echo('<font color=#bbbbcc>&quot;</font>'.$val.'<font color=#bbbbcc>&quot;</font>');
				else
					echo($val);
			}
		}
		echo('<font color=#bbbbcc>)</font>;<br>');
	}

	/**
	 * ...error
	 */
	public function error($as_string=true)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if($as_string)
			return $this->_error;
		if(strlen($this->_error))
			return true;
		return false;
	}

	/**
	 * ...loadForm
	 */
	public function loadForm($formname='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$formname=trim((string)$formname);

		// Load and parse configuration file / form
		if(!strlen($formname))
		{
			$formpath=$GLOBALS['datpath'].$this->_cid.'/cfg_prng.cfg';
			if(!file_exists($formpath))
				$formpath=$GLOBALS['datpath'].'0/cfg_prng.cfg';
		}
		else
		{
			$formpath=str_replace('\\','/', $formname);
			$fdp=stripos($formpath,'dat');
			$formpath=trim(substr($formpath,$fdp+3));
			if(substr($formpath,0,1) == '/')
				$formpath=substr($formpath,1);
			$formpath=$GLOBALS['datpath'].$formpath;
		}
		$fpa=load_pdfconfig(2, $formpath);

		$fpa['formname']=$formname;
		$fpa['formpath']=$formpath;

		return $fpa;
	}

	/**
	 * ...checkUse
	 *
	 * @param string  $formname - Optional (default=''):   Path and name for the client's invoice pdf form
	 * @param boolean $ret_text - Optional (default=true): Return text (not empty) or boolean (false) if Factur-X cannot be used
	 *
	 * @return string text or boolean
	 */
	public function checkUse($formname='', $ret_text=true)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		// Eine Factur-X / ZUGFeRD Rechnung kann leider nicht erstellt werden!|Sie können aber entweder eine normale PDF-Rechnung ohne XML-Anhang für E-Rechnungen erstellen oder aber die nachfolgend aufgelisteten Hinweise zuerst beheben bevor Sie weitermachen.|Die Klasse zum Erstellen von Factur-X Rechnungen konnte nicht eingebunden werden!|Nachfolgende Factur-X Codes in den Nachschlagewerten sind leer!|Nachfolgende Factur-X Werte im Rechnungsformular $var1<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fehlen oder sind leer!|Umsatzsteuer-ID|Steuernummer|Firmenname|Postleitzahl|Normalerweise Straßenname + Hausnummer|Name der Stadt|Länder-ID
		// Unfortunately, a Factur-X / ZUGFeRD invoice cannot be created!|However, you can either create a normal PDF invoice without an XML attachment for e-invoices or you can resolve the issues listed below before proceeding.|The class for creating Factur-X invoices could not be integrated!|The following Factur-X codes in the look-up values are empty!|The following Factur-X values in the invoice form $var1<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;are missing or empty!|VAT ID|Tax number|Company name|Postal code|Usually street name + number|City name|Country id
		$fta=explode('|', meldung(3428,false));
//fxDebug($fta,'$fta', 0);

		$fce='';

		// Check auf leere Factur-X Codes in den Nachschlagewerten: 63=Länderkennzeichen, 75=Währung + 1054=Verkaufseinheit
		$sql  = "SELECT id_feld, wertetabid, tabwert FROM wertetab";
		$sql .= " WHERE id_feld IN (63,75,1054) AND (codewert IS NULL OR codewert='') AND id_sprache=".$this->_lid." AND mandanten_id IN (0,".$this->_cid.")";
		$sql .= " ORDER BY id_feld, tabwert";
		$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql, 0);
		if(fxIsArray($tmp))
		{
			if(strlen($fce))
				$fce .= '<br />';
			// Nachfolgende Factur-X Codes in den Nachschlagewerten sind leer!<
			$fce .= '<table width="100%"><tr><td valign="top">&rarr;&nbsp;&nbsp;<b>'.$fta[3].'</b></td>';
			$fce .= '<td align="right" nowrap>&nbsp;&nbsp;&nbsp;'.fxpLink(58).'</td></tr></table>';
			$nif=0;
			foreach($tmp as $t)
			{
				$tif=(int)$t['id_feld'];
				if($tif != $nif)
				{
					$nif=$tif;
					$fce .= '<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span class=darkgrey>'.get_text($nif,'20',0,3).':</span><br />';
				}
				$fce .= '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&middot; <b>'.$t['tabwert'].'</b><br />';
			}
		}

		// Check auf leere Konfigurationsformparameter
		$fpa=$this->loadForm($formname);
		$formpath=$fpa['formpath'];
//fxDebug($fpa,'$fpa: $formpath='.$formpath, 0);
		$fec=0;
		$fea=array(5=>'v_ust', 'v_stn', 'fcx_Name', 'fcx_PostcodeCode', 'fcx_LineOne', 'fcx_CityName', 'fcx_CountryID');
		foreach($fea as $fti => $fek)
		{
			$flk=strtolower($fek);
			if(!isset($fpa[$flk]) || !strlen((string)$fpa[$flk]))
			{
				if(!$fec)
				{
					if(strlen($fce))
						$fce .= '<br />';
					$a_array=select_pdfconfig(2);
//fxDebug($a_array,'$a_array', 0);
					// Nachfolgende Factur-X Werte im Rechnungsformular $var1<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;fehlen oder sind leer!
					$fce .= '<table width="100%"><tr><td valign="top">&rarr;&nbsp;&nbsp;<b>'.str_replace('$var1', '<b class="blue">&quot;'.$a_array[$formname].'&quot;</b>', $fta[4]).'</b></td>';
					$fce .= '<td align="right" nowrap>&nbsp;&nbsp;&nbsp;'.fxpLink(217).'</td></tr></table><table>';
				}
				$fce .= '<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&middot; <b>'.$fek.'</b>&nbsp;</td><td>&nbsp;<i class="grey">'.$fta[$fti].'</i></td></tr>';
				$fec++;
			}
		}
		if($fec)
			$fce .= '</table>';

		if($ret_text)
			return $fce;
		else if(strlen($fce))
			return false;
		else
			return true;
	}

	/**
	 * ...getInvoiceData
	 *
	 * @param integer $id - Mandatory parameter: Invoice id
	 */
	public function getInvoiceData($iid=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		// Add some default values for testing
		$test=true;
		$test=false;

		// Set/Check invoice id
		$iid=(int)$iid;
		if($iid > 0)
			$this->_iid=$iid;
		if($this->_iid <= 0)
		{
			$this->_error='No invoice id set';
			return true;
		}

		// Get invoice data
		$sql="SELECT * FROM rechnung WHERE rechnungs_id=".$this->_iid." AND mandanten_id=".$this->_cid;
		$ida=db_values($sql);
//fxDebug($ida,'$ida<br />'.$sql, 0);
		if(!fxIsArray($ida))
		{
			$this->_error='No invoice with id [# '.$this->_iid.'] for client [# '.$this->_cid.'] found';
			return true;
		}

		// Load and parse configuration file / form
		$fpa=$this->loadForm($ida['rechnungsform']);
		$formpath=$fpa['formpath'];
//fxDebug($fpa,'$fpa: $formpath='.$formpath, 0);

		// Execute invoice programm to get correct formated valid positions
		$smsk277=true;
		$Rechnungs_ID_akt=$this->_iid;
		$asFacX=true;
		$req_fn=fxf_fn_reqFilename('rchg_4_rep_pre', 104);
		if(strlen((string)$req_fn))
			require($req_fn);
		if(!strlen((string)$req_fn) || !isset($mc) || is_null($mc) || !fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
		{
			$this->_error='Unable to get invoice positions for invoice with id [# '.$this->_iid.'] for client [# '.$this->_cid.'] by executing the SPP [rchg_4_rep_pre]';
			return true;
		}
//fxDebug($GLOBALS['fxptdata']['lvalues'][$mc],'$GLOBALS[\'fxptdata\'][\'lvalues\']['.$mc.']: $req_fn='.$req_fn, 0);

		// Get client's currency id, code and value
		$cui=getCurrency(0, $this->_cid, true);
		$cua=valueCode($cui, true, false, $this->_lid);
//fxDebug($cua,'$cua: $cui='.$cui, 0);
		if(!fxIsArray($cua) || !isset($cua['code']) || !strlen((string)$cua['code']))
		{
			$this->_error='Unable to get currency code for client [# '.$this->_cid.'] in language [# '.$this->_lid.']';
			return true;
		}

		// Datumsformat ermitteln, falls noch nich gesetzt
		if(!fxIsArray($GLOBALS['fxpglobals']['settings']) || !isset($GLOBALS['fxpglobals']['settings']['d_formatdatum']))
		{
			// Benutzer-ID des Rechnungserstellers ermitteln
			$uid=(int)db_value("SELECT benutzer_id FROM benutzer WHERE personen_id=".$ida['aenderungs_id']." AND mandanten_id=".$this->_cid);
			$GLOBALS['fxpglobals']['settings']=benutzereinstellungen($uid);
		}

		// Get codes 63=country codes + 1054=units
		$ca63=valueCode(63, true, true, $this->_lid);
//fxDebug($ca63,'$ca63', 0);
		$ca1054=valueCode(1054, true, true, $this->_lid);
//fxDebug($ca1054,'$ca1054', 0);

		// Anmerkung: Der Rechnungstyp muss gemäß UNTDID 1001, spezifiziert werden
		// 326 (Partial invoice), 380 (Commercial invoice), 384 (Corrected invoice), 389 (Self-billed invoice), 381 (Credit note), 875 (Partial construction invoice), 876 (Partial final construction invoice), 877 (Final construction invoice)
		// -> 326=Teilrechnung (falls prozentualer Festpreis), 380=Rechnung/Schlußrechnung, 384=Storno
		$ifact=1.0;
		$otype=380;	// 380=Commercial invoice/Handelsrechnung/Rechnung/Schlußrechnung (Standard)
		$itype=$otype;
		if((int)$ida['kalktyp'] != 2734)	// 2734=Komplett
		{
			// Alle Positionen durchgehen um nur teilweise abgerechnete Festpreise zu ermitteln
			foreach($GLOBALS['fxptdata']['lvalues'][$mc] as $ip)
			{
				if(($ip['pid'] > 0) && (($ip['typ'] == 50) || ($ip['typ'] == 54)) && ($ip['r_gesamtpreis'] < $ip['r_einzelpreis']))	// 50=Festpreis Personen + 54=Festpreis Material
				{
					$otype=326;	// 326=Partial invoice/Teilrechnung (falls prozentualer Festpreis)
					$itype=$otype;
					break;
				}
			}
		}
		if((int)$ida['archiv'])
		{
			$ifact=-1.0;
			$itype=384;	// 384=Corrected invoice/Rechnungskorrektur/Storno
		}

		// Set values
		$this->_fxa=array(
			'idata'		=> $ida,							// Rechnungsdatenarray aus DB
			'ipdf'		=> $fpa,							// PDF-Rechnungskonfiguration

			'number'	=> $ida['rechnn_f'],				// Rechnungsnummer
			'otype'		=> $otype,							// Ursprungscode für den Rechnungstyp [fxcInvoiceType]  (Sheet "1001")
			'itype'		=> $itype,							// Anzuzeigender Code für den Rechnungstyp (Ändert sich nur bei Storno) [fxcInvoiceType]  (Sheet "1001")
			'ifact'		=> $ifact,							// Fakturierung: -1.0 bei Storno
			'idate'		=> substr($ida['datum'],0,8),		// Rechnungsdatum (Format 102 verpflichtend = YYYYMMDD)
			'ddate'		=> substr($ida['datum'],0,8),		// Lieferdatum (optional / obligatorisch falls es vom Rechnungsdatum abweicht)
			'pdate'		=> "",								// Fälligkeitsdatum / Stichtag
			'currency'	=> $cua['code'],					// Currency code [fxcCurrencyID]: InvoiceCurrencyCode  (Sheet "Currency")
			'contract'	=> "",								// ContractReferencedDocument = Vertragsnummer
			'sorder'	=> "",								// SellerOrderReferencedDocument = Auftragsnummer
			'border'	=> "",								// BuyerOrderReferencedDocument = Bestellnummer
			'timespan'	=> array(),							// BillingSpecifiedPeriod / Abrechnungszeitraum
			'project'	=> array(),							// SpecifiedProcuringProject: Projektangaben
			'notes'		=> array(),							// Include Notes(s) / Bemerkung(en) (String starting with * means  REG=Regulatory information)
			'positions'	=> array(),							// IncludedSupplyChainTradeLineItem(s) / Rechnungsposition(en)
			'discounts'	=> array(),							// Rabatt-/Nachlassposition(en)
			'prepaids'	=> array(),							// Vorauszahlungsposition(en)
			'vats'		=> array(),							// ApplicableTradeTax / Umsatzsteueraufschlüsselung(en)
			'client'	=> array(),							// SellerTradeParty / Verkäufer / Rechnungsersteller / Mandant
			'customer'	=> array(),							// BuyerTradeParty / Käufer / Rechnungsempfänger / Auftraggeber
			'sum'		=> array(),							// SpecifiedTradeSettlementHeaderMonetarySummation / Rechnungssumme
			'terms'		=> '',								// SpecifiedTradePaymentTerms / Zahlungsangaben / Skonto
			'payments'	=> array()							// PaymentMeans / Zahlungsmittel
		);

		// Lieferdatum
		if(strlen((string)$ida['ausgegangen_am']))
			$this->_fxa['ddate']=substr($ida['ausgegangen_am'],0,8);

		// Fälligkeitssdatum
		if(strlen((string)$ida['stichtag']))
			$this->_fxa['pdate']=substr($ida['stichtag'],0,8);
		if($test && !strlen($this->_fxa['pdate']))
			$this->_fxa['pdate']=$GLOBALS['date'];

		// Projektangaben
		if((int)$ida['mutterprojekt_id'] > 0)
		{
			$this->_fxa['project']['id']='FXP/'.$this->_cid.'/'.(int)$ida['mutterprojekt_id'];
			$this->_fxa['project']['name']=(string)$ida['name_mutterprojekt'];
			$pva=db_values("SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".(int)$ida['mutterprojekt_id']." AND mandanten_id=".$this->_cid);
//fxDebug($pva,'$pva: project='.(int)$ida['mutterprojekt_id'], 0);
			if(fxIsArray($pva))
			{
				$this->_fxa['project']['id'] .= '/'.no_null($pva['vorgangsnummer']);
				if(!strlen($this->_fxa['project']['name']))
					$this->_fxa['project']['name']=$pva['name_projekt'];
			}
		}

		// InvoicePositions + ApplicableTradeTax / Umsatzsteueraufschlüsselung(en)
		$pda=array(
			'<br />'=>$GLOBALS['nl_b'],
			'<u>'=>"", '</u>'=>"", '<b>'=>"", '</b>'=>"", '<i>'=>"", '</i>'=>"",
			'&nbsp;'=>" ", '&apos;'=>"'", '&delta;'=>"ø", '&Delta;'=>"ø", '&sum;'=>"+", '&middot;'=>"-", '&uarr;'=>"^", '&darr;'=>"¦", '&larr;'=>"«", '&rarr;'=>"»", '&amp;'=>"&"
		);
		$txt_percent=get_text(1584,'15');	// Prozent (%)
		$spos=0.0;							// Gesamtsumme aller Positionen (netto)
		$schg=0.0;							// Gesamtsumme aller Zuschläge (netto)
		$sall=0.0;							// Gesamtsumme aller Nachlässe (netto)
		$svat=0.0;							// Gesamtsumme aller Mehrwertsteuern (netto)
		$spre=0.0;							// Gesamtsumme aller Vorauszahlungen (brutto)
		$snts='';
		foreach($GLOBALS['fxptdata']['lvalues'][$mc] as $ip)
		{
			// Positionsnummer
			$a=array(
				'line'		=> $ip['positions_id'],
				'gid'		=> 'FXP/'.$ip['gid'],
				'sid'		=> 'FXP/'.$ip['sid'],
				'typ'		=> $ip['typ'],
				'pid'		=> $ip['pid'],
				'rid'		=> $ip['rid'],
				'aid'		=> $ip['aid'],
				'bnd'		=> $ip['bnd'],
				'name'		=> $ip['r_bezeichnung'],
				'desc'		=> $ip['anmerkung'],
				'amount'	=> $ip['r_anzahl']*$ifact,
				'unit'		=> $ip['verkaufseinheit'],
				'unitid'	=> $ip['unitid'],
				'unitcode'	=> $ca1054[$ip['unitid']]['code'],
				'unet'		=> $ip['r_einzelpreis'],
				'gnet'		=> round($ip['r_gesamtbrutto'] / $ip['r_anzahl'], 4),
				'tnet'		=> $ip['r_gesamtpreis']*$ifact,
				'vcat'		=> 'S',	// Code der Umsatzsteuerkategorie [fxcCategoryCode]: S=Standard rate  (Sheet "5305")
				'pvat'		=> $ip['mwst_satz'],
				'tvat'		=> $ip['r_gesamtmwst']*$ifact,
				'tgross'	=> $ip['r_gesamtbrutto']*$ifact
			);
			// Keine Mehrwertsteuer, dann Code der Umsatzsteuerkategorie [fxcCategoryCode]: Z=Zero rated goods  (Sheet "5305")
			if(!$ip['mwst_satz'])
				$a['vcat']='Z';
			// Festpreis, dann Einheit abändern auf Prozent + Listenpreis auf Gesamtpreis
			if(($ip['typ'] == 50) || ($ip['typ'] == 54))
			{
				$a['unit']=$txt_percent.' / '.$a['unit'];
				$a['unitcode']='P1';	// P1=percent
				$a['gnet']=round($ip['r_einzelpreis'] * (float)('1.'.str_replace('.','', (string)$ip['mwst_satz'])), 4);
			}
			// HTML-Sonderzeichen aus Texten entfernen
			if(strlen((string)$ip['r_bezeichnung']))
				$a['name']=trim(strtr($ip['r_bezeichnung'], $pda));
			if(strlen((string)$ip['anmerkung']))
				$a['desc']=trim(strtr($ip['anmerkung'], $pda));
			// Umsatzsteuersatz: Umsatzsteuerkategorie / Nettobetrag der Rechnungsposition / Umsatzsteuerbetrag
			$vr=$this->fxcRound($ip['mwst_satz'],2);
			if(!isset($this->_fxa['vats'][$vr]))
				$this->_fxa['vats'][$vr]=array('vcat'=>'S', 'tnet'=>$ip['r_gesamtpreis']*$ifact, 'tvat'=>$ip['r_gesamtmwst']*$ifact);
			else
			{
				$this->_fxa['vats'][$vr]['tnet'] += $ip['r_gesamtpreis']*$ifact;
				$this->_fxa['vats'][$vr]['tvat'] += $ip['r_gesamtmwst']*$ifact;
			}
			// Zuordnungstyp
			if(!$a['typ'])				// Vorauszahlunsposition
			{
				$this->_fxa['prepaids'][$a['line']]=$a;
				$spre -= $a['tgross']; // da negativer Wert
				if(strlen($snts))
					$snts .= $GLOBALS['nl_b'];
				$snts .= $a['name'].' = '.get_entry(-1.0 * $a['tgross'], 'decimal').' '.$cua['value'];
			}
			else
			{
				$svat += $a['tvat'];
				if($a['unet'] < 0.0)	// Rabatt-/Nachlassposition
				{
					$this->_fxa['discounts'][$a['line']]=$a;
					$sall += $a['tnet'];
				}
				else					// Rechnungsposition
				{
					$this->_fxa['positions'][$a['line']]=$a;
					$spos += $a['tnet'];
				}
			}
		}
		if(sizeof($this->_fxa['vats']))
			ksort($this->_fxa['vats']);

		// InvoiceNotes
		// ...Rechnungstyp
		$it=(int)$ida['rechnungstyp'];
		$in='';
		if($it > 0)
			$in=db_value("SELECT kommunikation_txt FROM kommunikation WHERE kommunikation_id=".$it." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		else if(($otype == 326) || ($ida['kalktyp'] == 2734))	// Teilrechung oder Schlußrechnung
		{
			$ia=explode('|', meldung(3430,false));	// Rechnung|Rechnungskorrektur|Teilrechung|Schlußrechnung
			if($otype == 326)						// ...Teilrechung
				$in=$ia[2];
			else									// ...Schlußrechnung
				$in=$ia[3];
		}
		if($itype == 384)							// 384=Corrected invoice/Rechnungskorrektur/Storno
		{
			$st=meldung(3092,false);
			if(strlen($in))
				$in .= ' ['.$st.']';
			else
				$in=$st;
		}
		if(strlen($in))
			$this->_fxa['notes']['*'.meldung(2121, true).'|invtype']=$in;

		// ...Zeitspanne
		$rbd='';
		if(strlen((string)$ida['ist_beg_dtm']))
		{
			$rbd=trim((string)$ida['ist_beg_dtm']);
			$this->_fxa['timespan']['start']=substr($rbd,0,8);
		}
		if(strlen((string)$rbd))
			$rbd=get_entry($rbd, 'date');
		$red='';
		if(strlen((string)$ida['ist_end_dtm']))
		{
			$red=trim((string)$ida['ist_end_dtm']);
			$this->_fxa['timespan']['end']=substr($red,0,8);
		}
		if(strlen((string)$red))
			$red=get_entry($red, 'date');
		if(strlen($rbd) || strlen($red))
		{
			if(!strlen($rbd))
				$rbd='*';
			if(!strlen($red))
				$red='*';
			$this->_fxa['notes']['*'.meldung(1105, true).'|timespan']=$rbd.'-'.$red;
		}
		// ...KTO's
		if(strlen((string)$ida['kto']))
		{
			$a=explode(',', $ida['kto']);
			foreach($a as $v)
			{
				$e=explode('_', $v);
				$i=(int)$e[0];
				if(($i == 2113) || ($i == 2121))	// Rechnungsanschrift + Rechnungstyp überpringen
					continue;

				$m=meldung($i, false);
				$x=db_value("SELECT kommunikation_txt FROM kommunikation WHERE kommunikation_id=".(int)$e[1]." AND mandanten_id=".$this->_cid);
				$this->_fxa['notes']['*'.$m.'|kto_'.$i]=$x;
			}
		}
		// ...Projektname
		if(strlen((string)$ida['name_mutterprojekt']))
			$this->_fxa['notes']['*'.get_text(814,'20').'|projectname']=$ida['name_mutterprojekt'];
		// ...Text Zusatz (Kopf)
		$lit_note=get_text(44,'15');	// Anmerkung
		$nc=1;
		if(strlen((string)$ida['allg_textfeld']))
		{
			$uc=1;
			$ta=explode('{nl}', strtr($ida['allg_textfeld'], array("\n\r"=>'{nl}', "\n"=>'{nl}')));
//fxDebug($ta,'$ta', 0);
			foreach($ta as $t)
			{
				$t=trim($t);
				if(strlen($t))
				{
					$cp=strpos($t,':');
					if($cp > 0)
						$this->_fxa['notes']['*'.trim(substr($t,0,$cp)).'|textaddon_head_'.$uc]=trim(substr($t,$cp+1));
					else
					{
						$this->_fxa['notes']['*'.$lit_note.' '.$nc.'|textaddon_head_'.$uc]=$t;
						$nc++;
					}
					$uc++;
				}
			}
		}
		// ...PDF Formular: fcx_note(s)
		foreach($fpa as $fpk => $fpv)
		{
			$uc=1;
			$t=trim($fpv);
			if((substr($fpk,0,8) == 'fcx_note') && strlen($t))
			{
				$cp=strpos($t,':');
				if($cp > 0)
					$this->_fxa['notes']['*'.trim(substr($t,0,$cp)).'|fxc_note_'.$uc]=trim(substr($t,$cp+1));
				else
				{
					$this->_fxa['notes']['*'.$lit_note.' '.$nc.'|fxc_note_'.$uc]=$t;
					$nc++;
				}
				$uc++;
			}
		}
		// ...ABT: Information/instructions about additional amounts covered
		if(strlen($snts))
			$this->_fxa['notes']['*ABT']=$snts;	// Code zur Qualifizierung des Freitextes [fxcSubjectCode]: ABT: Information/instructions about additional amounts covered  (Sheet "Text")
		// ...AAK: Es bestehen Rabatt- und Bonusvereinbarungen.
		if($sall != 0.0)
			$this->_fxa['notes']['*AAK']=meldung(3431,false);	// Code zur Qualifizierung des Freitextes [fxcSubjectCode]: AAK=Regulatory information  (Sheet "Text")

		// ...PDF Formular: v_text
		if(isset($fpa['v_text']) && strlen(trim((string)$fpa['v_text'])))
		{
			if(strlen($this->_fxa['terms']))
				$this->_fxa['terms'] .= $GLOBALS['nl_b'];
			$this->_fxa['terms'] .= trim((string)$fpa['v_text']);
		}
		// ...Text Zusatz (Fuss)
		if(strlen((string)$ida['allg_textfeld_fuss']))
		{
			$uc=1;
			$ta=explode('{nl}', strtr($ida['allg_textfeld_fuss'], array("\n\r"=>'{nl}', "\n"=>'{nl}')));
//fxDebug($ta,'$ta', 0);
			foreach($ta as $t)
			{
				$t=trim($t);
				if(strlen($t))
				{
					if(strlen($this->_fxa['terms']))
						$this->_fxa['terms'] .= $GLOBALS['nl_b'];
					$this->_fxa['terms'] .= $ida['allg_textfeld_fuss'];
					$uc++;
				}
			}
		}
		// ...KTU's
		if(strlen((string)$ida['ktu']))
		{
			$a=explode(',', $ida['ktu']);
			foreach($a as $v)
			{
				$e=explode('_', $v);
				$i=(int)$e[0];
				$x=db_value("SELECT kommunikation_txt FROM kommunikation WHERE kommunikation_id=".(int)$e[1]." AND mandanten_id=".$this->_cid);
				if(strlen($this->_fxa['terms']))
					$this->_fxa['terms'] .= $GLOBALS['nl_b'];
				$this->_fxa['terms'] .= $x;
			}
		}

		// Lieferdatum, Vertragsnummer, Auftragsnummer oder Bestellnummer (in Notes)
		if(fxIsArray($this->_fxa['notes']))
		{
			foreach($this->_fxa['notes'] as $nid => $ntx)
			{
				// Lieferdatum
				if((stripos($nid,'lieferdatum') !== false) || (stripos($nid,'delivery date') !== false))
					$this->_fxa['ddate']=substr(strtodatetime($GLOBALS['fxpglobals']['settings']['d_formatdatum'],$ntx), 0,8);
				// Vertragsnummer
				if(!strlen($this->_fxa['contract']) && ((stripos($nid,'vertrag') !== false) || (stripos($nid,'contract') !== false)))
					$this->_fxa['contract']=$ntx;
				// Auftragsnummer
				if(!strlen($this->_fxa['sorder']) && ((stripos($nid,'auftrag') !== false) || (stripos($nid,'track') !== false) || (stripos($nid,'job') !== false)))
					$this->_fxa['sorder']=$ntx;
				// Bestellnummer
				if(!strlen($this->_fxa['border']) && ((stripos($nid,'bestell') !== false) || (stripos($nid,'order') !== false)))
					$this->_fxa['border']=$ntx;
			}
		}
		if($test && !strlen($this->_fxa['contract']))
			$this->_fxa['contract']='Contract number 123 456';
		if($test && !strlen($this->_fxa['sorder']))
			$this->_fxa['sorder']='Seller order number 123 456';
		if($test && !strlen($this->_fxa['border']))
			$this->_fxa['border']='Buyer order number 123 456';

		// Verkäufer / Rechnungsersteller / Mandant
		$cta=array('gid'=>"globalid", 'nam'=>"name", 'cnt'=>"contact", 'tel'=>"telephone", 'eml'=>"email", 'pst'=>"postcodecode", 'ln1'=>"lineone", 'ln2'=>"linetwo", 'ln3'=>"linethree", 'cty'=>"cityname", 'ctr'=>"countryid", 'sdn'=>"region");
		$this->_fxa['client']=array('cid'=>'FXP/'.$ida['mandanten_id'], 'gis'=>"", 'gid'=>"", 'ini'=>"", 'nam'=>"", 'cnt'=>"", 'tel'=>"", 'eml'=>"", 'pst'=>"", 'ln1'=>"", 'ln2'=>"", 'ln3'=>"", 'cty'=>"", 'ctr'=>"", 'sdn'=>"", 'tfc'=>"", 'tva'=>"");
		// ...Info from database, here: cid
		$cpa=db_values("SELECT personen_id, personen_initialen FROM personen WHERE personentyp=274 AND mandanten_id=".$this->_cid);
		$cpi=(int)$cpa['personen_id'];
//fxDebug($cpa,'$cpa: $cpi='.$cpi, 0);
		$this->_fxa['client']['cid'] .= '/'.$cpi;
		if(strlen((string)$cpa['personen_initialen']))
		{
			$this->_fxa['client']['ini']=$cpa['personen_initialen'];
			$this->_fxa['client']['cid'] .= '/'.$cpa['personen_initialen'];
		}
		else
			$this->_fxa['client']['cid'] .= '/NN';
		$this->_fxa['client']['cid'] .= '/'.$ida['aenderungs_id'];
		// ...Info from PDF config file, here: 'adr' in head section
		if(isset($fpa['head']) && strlen($fpa['head']))
		{
			$hex=explode('^', $fpa['head']);
			foreach($hex as $h)
			{
				if(substr($h,0,4) == '!adr')
				{
					$pp=strpos($h,'|',4);
					if($pp > 0)
					{
						$aex=explode(',', substr($h,$pp+1));
						$this->_fxa['client']['nam']=trim($aex[0]);
						if(sizeof($aex) > 1)
							$this->_fxa['client']['ln1']=trim($aex[1]);
						if(sizeof($aex) > 2)
						{
							$sp=strpos($aex[2],' ',2);
							if($sp > 0)
							{
								$this->_fxa['client']['pst']=trim(substr($aex[2],0,$sp));
								$this->_fxa['client']['cty']=trim(substr($aex[2],$sp+1));
							}
						}
					}
				}
			}
		}
		// ...Info from PDF config file, here: v_stn + v_ust
		if(isset($fpa['v_stn']) && strlen($fpa['v_stn']))
			$this->_fxa['client']['tfc']=$fpa['v_stn'];
		if(isset($fpa['v_ust']) && strlen($fpa['v_ust']))
			$this->_fxa['client']['tva']=$fpa['v_ust'];
		// ...Info from PDF config file, here: fcx values
		foreach($cta as $cky => $cvt)
		{
			if(isset($fpa['fcx_'.$cvt]) && strlen((string)$fpa['fcx_'.$cvt]))
				$this->_fxa['client'][$cky]=$fpa['fcx_'.$cvt];
			if(strlen($this->_fxa['client']['gid']))
			{
				$pp=strpos($this->_fxa['client']['gid'],'|');
				if($pp > 0)
				{
					$this->_fxa['client']['gis']=substr('0000'.substr($this->_fxa['client']['gid'],0,$pp),-4);
					$this->_fxa['client']['gid']=substr($this->_fxa['client']['gid'],$pp+1);
				}
			}
		}

		// Käufer / Rechnungsempfängers / Auftraggeber
		$this->_fxa['customer']=array('cid'=>'FXP/'.$ida['mandanten_id'].'/'.$ida['kunde'], 'ini'=>"", 'nam'=>"", 'cnt'=>"", 'tel'=>"", 'eml'=>"", 'pst'=>"", 'ln1'=>"", 'ln2'=>"", 'ln3'=>"", 'cty'=>"", 'ctr'=>"");
		$ina=array();
		// ...ID
		if(strlen((string)$ida['kundennummer']))
		{
			$this->_fxa['customer']['ini']=$ida['kundennummer'];
			$this->_fxa['customer']['cid'] .= '/'.$ida['kundennummer'];
		}
		else
			$this->_fxa['customer']['cid'] .= '/NN';
		$this->_fxa['customer']['cid'] .= '/'.(int)$ida['person_angelegt'];
		// ...Anrede Auftraggeber
		if(($ida !== NULL) && strlen((string)trim((string)$ida['anredetext'])))
			$ina[]=trim((string)$ida['anredetext']);
		// ...Name Auftraggeber
		$val=trim((string)$ida['pname']);
		if(($ida['vorname'] !== NULL) && strlen((string)trim((string)$ida['vorname'])))
			$val=trim((string)$ida['vorname']).' '.$val;
		$ina[]=$val;
		$this->_fxa['customer']['nam']=$val;
		// ...Anrede+Name Ansprechpartner+Initialen+Telefonnummer+E-Mail-Adresse
		if((int)$ida['person_angelegt'] > 0)
		{
			$sql="SELECT elter, anrede, perstitel, pname, vorname, name_zusatz FROM personen WHERE personen_id=".(int)$ida['person_angelegt']." AND mandanten_id=".$this->_cid;
			$caa=db_values($sql);
//fxDebug($caa,'$caa', 0);
			if(fxIsArray($caa))
			{
				$val=trim((string)$caa['pname']);
				if(strlen((string)trim((string)$caa['vorname'])))
					$val=trim((string)$caa['vorname']).' '.$val;
				if((int)$caa['perstitel'] > 0)
					$val=meldung((int)$caa['perstitel'], true).' '.$val;
				$i=(int)$caa['anrede'];
				if(($i > 0) && ($i != 310) && ($i != 2803))	// Alle außer 310=Firma + 2803=Herr/Frau
					$val=meldung($i, true).' '.$val;
				$ina[]=$val;
				$this->_fxa['customer']['cnt']=$val;
				// ...Zusatz Ansprechpartner
				if(($caa['name_zusatz'] !== NULL) && strlen((string)trim((string)$caa['name_zusatz'])))
				{
					$ina[]=trim((string)$caa['name_zusatz']);
					$this->_fxa['customer']['cnt'] .= ' - '.trim((string)$caa['name_zusatz']);
				}
			}
			// ...2365: Standard Phone Number + 2366: Standard Email Address
			$pea=array((int)$ida['person_angelegt']=>array(2365=>'tel', 2366=>'eml'));
			if(fxIsArray($caa) && ((int)$caa['elter'] > 0))
				$pea[(int)$caa['elter']]=array(2365=>'tel', 2366=>'eml');
//fxDebug($pea,'$pea', 0);
			foreach($pea as $pei => $psa)
			{
				foreach($psa as $psi => $pst)
				{
					if(strlen($this->_fxa['customer'][$pst]))
						break;
					$val=(string)db_value("SELECT kommunikation_txt FROM kommunikation WHERE personen_id=".$pei." AND mandanten_id=".$this->_cid." AND kommunikationstyp=".$psi);
//fxDebug($val,'$val: pers='.$pei.', typ='.$psi, 0);
					if(strlen($val))
						$this->_fxa['customer'][$pst]=$val;
				}
			}
		}
		// ...Strasse
		$ina[]=trim((string)$ida['strasse']);
		$this->_fxa['customer']['ln1']=trim((string)$ida['strasse']);
		// ...LKZ + PLZ + Ort
		$val=trim((string)$ida['ort']);
		$this->_fxa['customer']['cty']=$val;
		if(strlen((string)trim((string)$ida['plz'])))
		{
			$po=trim((string)$ida['plz']);
			$this->_fxa['customer']['pst']=$po;
			if(strlen(trim((string)$ida['staat'])))
			{
				$cct=trim((string)$ida['staat']);
				if(fxIsArray($ca63))
				{
					foreach($ca63 as $c)
					{
						if($c['value'] == $cct)
							$this->_fxa['customer']['ctr']=$c['code'];
					}
				}
				$po=$cct.'-'.$po;
			}
			$val=$po.' '.$val;
		}
		$ina[]=$val;
		$this->_fxa['notes']['*REG']=implode($GLOBALS['nl_b'], $ina);	// Code zur Qualifizierung des Freitextes [fxcSubjectCode]: REG=Regulatory information  (Sheet "Text")

		// SpecifiedTradeSettlementHeaderMonetarySummation / Rechnungssumme
		// ...LineTotalAmount / Gesamtnettowert vor Steuern
		$this->_fxa['sum']['pos']=$spos;
		// ...ChargeTotalAmount / Summe der Abgaben (= Zuschläge) auf Dokumentebene (optional)
		$this->_fxa['sum']['chg']=$schg;
		// ...AllowanceTotalAmount / Summe der Freibeträge (= Nachlässe) auf Dokumentebene (optional)
		$this->_fxa['sum']['all']=$sall;
		// ...TaxBasisTotalAmount / Betrag ohne Steuern
		$this->_fxa['sum']['net']=$this->_fxa['sum']['pos']+$this->_fxa['sum']['chg']+$this->_fxa['sum']['all'];
		// ...TaxTotalAmount / Gesamtbetrag der Steuern
		$this->_fxa['sum']['vat']=$svat;
		// ...GrandTotalAmount / Gesamtbruttobetrag inkl. Steuern
		$this->_fxa['sum']['grs']=$this->_fxa['sum']['net']+$this->_fxa['sum']['vat'];
		// ...TotalPrepaidAmount / Vorauszahlungsbetrag (brutto)
		$this->_fxa['sum']['pre']=$spre;
		// ...DuePayableAmount / Zu zahlender Nettobetrag (laut Doku), aber im Beispiel ist es der Bruttobetrag, was mehr Sinn macht ???
		$this->_fxa['sum']['pay']=$this->_fxa['sum']['grs']-$this->_fxa['sum']['pre'];

		// PaymentMeans / Zahlungsmittel
		// ...Info from PDF config file, here: fcx values
		if(isset($fpa['fcx_iban']) && strlen($fpa['fcx_iban']))
		{
			// Accounting
			$this->_fxa['payments']=array('iban'=>$fpa['fcx_iban'], 'aname'=>"", 'bic'=>"");
			if(isset($fpa['fcx_accountname']) && strlen($fpa['fcx_accountname']))
				$this->_fxa['payments']['aname']=$fpa['fcx_accountname'];
			if(isset($fpa['fcx_bic']) && strlen($fpa['fcx_bic']))
				$this->_fxa['payments']['bic']=$fpa['fcx_bic'];
		}
//fxDebug($this->_fxa,'$this->_fxa', 0);

		return false;
	}

	/**
	 * ...createFacturX
	 *
	 * @return string Factur-X, i.e. XML
	 */
	public function createFacturX($save=false)
	{
// Value added tax point date code: 3=Invoice document issue date time
// Term: #SKONTO#TAGE=14#PROZENT=2.25#

		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$lit_unit=get_text(1054,'10');	// Einheit

		$xml  = '<?xml version="1.0" encoding="UTF-8" ?>'.$GLOBALS['nl_b'];
		$xml .= '<rsm:CrossIndustryInvoice xmlns:rsm="urn:un:unece:uncefact:data:standard:CrossIndustryInvoice:100" xmlns:qdt="urn:un:unece:uncefact:data:standard:QualifiedDataType:100" xmlns:ram="urn:un:unece:uncefact:data:standard:ReusableAggregateBusinessInformationEntity:100" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:udt="urn:un:unece:uncefact:data:standard:UnqualifiedDataType:100">'.$GLOBALS['nl_b'];
		$xml .= '  <rsm:ExchangedDocumentContext>'.$GLOBALS['nl_b'];
		$xml .= '    <ram:GuidelineSpecifiedDocumentContextParameter>'.$GLOBALS['nl_b'];
		// ...Kompatibilitätskennzeichen
		$xml .= '      <ram:ID>urn:cen.eu:en16931:2017</ram:ID>'.$GLOBALS['nl_b'];
		$xml .= '    </ram:GuidelineSpecifiedDocumentContextParameter>'.$GLOBALS['nl_b'];
		$xml .= '  </rsm:ExchangedDocumentContext>'.$GLOBALS['nl_b'];

		// ExchangedDocument
		$xml .= '  <rsm:ExchangedDocument>'.$GLOBALS['nl_b'];
		// ...Rechnungsnummer
		$xml .= '    <ram:ID>'.$this->fxcText($this->_fxa['number']).'</ram:ID>'.$GLOBALS['nl_b'];
		// ...Code für den Rechnungstyp [fxcInvoiceType]
		$xml .= '    <ram:TypeCode>'.$this->_fxa['itype'].'</ram:TypeCode>'.$GLOBALS['nl_b'];
		$xml .= '    <ram:IssueDateTime>'.$GLOBALS['nl_b'];
		// ...Rechnungsdatum
		$xml .= '      <udt:DateTimeString format="102">'.$this->_fxa['idate'].'</udt:DateTimeString>'.$GLOBALS['nl_b'];
		$xml .= '    </ram:IssueDateTime>'.$GLOBALS['nl_b'];
		// ... IncludedNote(s) / Bemerkung(en) / Freitext zur Rechnung
		if(sizeof($this->_fxa['notes']))
		{
			foreach($this->_fxa['notes'] as $ni => $nt)
			{
				$xml .= '    <ram:IncludedNote>'.$GLOBALS['nl_b'];
				$xml .= '      <ram:Content>'.$this->fxcText($nt).'</ram:Content>'.$GLOBALS['nl_b'];
				if(substr($ni,0,1) == '*')
				{
					$ni=substr($ni,1);
					$pp=strpos($ni,'|');
					if($pp)
						$ni=substr($ni,0,$pp);
					$xml .= '      <ram:SubjectCode>'.$this->fxcText($ni).'</ram:SubjectCode>'.$GLOBALS['nl_b'];
				}
				$xml .= '    </ram:IncludedNote>'.$GLOBALS['nl_b'];
			}
		}
		$xml .= '  </rsm:ExchangedDocument>'.$GLOBALS['nl_b'];

		// SupplyChainTradeTransaction / Rechnungspositionen
		$xml .= '  <rsm:SupplyChainTradeTransaction>'.$GLOBALS['nl_b'];
		if(sizeof($this->_fxa['positions']))
		{
			foreach($this->_fxa['positions'] as $pa)
			{
				// ...Rechnungsposition
				$xml .= '    <ram:IncludedSupplyChainTradeLineItem>'.$GLOBALS['nl_b'];

				$xml .= '      <ram:AssociatedDocumentLineDocument>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:LineID>'.$pa['line'].'</ram:LineID>'.$GLOBALS['nl_b'];						// Kennung der Rechnungsposition / Positionsnummer
				// ......Freitext
				$inc=$lit_unit.': '.$pa['unit'].' ('.$pa['unitcode'].' = '.$this->fxcUnitCode($pa['unitcode']).')';	// (Anmerkung) + Einheit
				if(strlen((string)$pa['desc']))
					$inc=$pa['desc'].$GLOBALS['nl_b'].$inc;
				$xml .= '        <ram:IncludedNote>'.$GLOBALS['nl_b'];
				$xml .= '          <ram:Content >'.$this->fxcText($inc).'</ram:Content >'.$GLOBALS['nl_b'];			// Freitext = Anmerkung
				$xml .= '        </ram:IncludedNote>'.$GLOBALS['nl_b'];
				$xml .= '      </ram:AssociatedDocumentLineDocument>'.$GLOBALS['nl_b'];
				// ......Artikelinformation
				$xml .= '      <ram:SpecifiedTradeProduct>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:GlobalID schemeID="0221">'.$pa['gid'].'</ram:GlobalID>'.$GLOBALS['nl_b'];		// Kennung eines Artikels nach registriertem Schema [fxcSchemeID]: 0160=GTIN, evtl. '0236'=>'Description not known' oder '0111'=>'Object Identifiers' oder '0221'=>'The registered number of the qualified invoice issuer'  (Sheet "ICD")
				$xml .= '        <ram:SellerAssignedID>'.$pa['sid'].'</ram:SellerAssignedID>'.$GLOBALS['nl_b'];		// Artikelkennung des Verkäufers
				$xml .= '        <ram:Name>'.$this->fxcText($pa['name']).'</ram:Name>'.$GLOBALS['nl_b'];			// Artikelname
//				if(strlen((string)$pa['desc']))
//					$xml .= '        <ram:Description>'.$pa['desc'].'</ram:Description>'.$GLOBALS['nl_b'];			// Artikelbeschreibung
				$xml .= '      </ram:SpecifiedTradeProduct>'.$GLOBALS['nl_b'];
				// ......Detailinformationen zum Preis
				$xml .= '      <ram:SpecifiedLineTradeAgreement>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:GrossPriceProductTradePrice>'.$GLOBALS['nl_b'];
				$xml .= '          <ram:ChargeAmount>'.$this->fxcRound($pa['gnet'],4).'</ram:ChargeAmount>'.$GLOBALS['nl_b'];	// Bruttopreis des Artikels
				$xml .= '          <ram:BasisQuantity unitCode="'.$pa['unitcode'].'">'.$this->fxcRound(1,4).'</ram:BasisQuantity>'.$GLOBALS['nl_b'];
				$xml .= '        </ram:GrossPriceProductTradePrice>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:NetPriceProductTradePrice>'.$GLOBALS['nl_b'];
				$xml .= '          <ram:ChargeAmount>'.$this->fxcRound($pa['unet'],4).'</ram:ChargeAmount>'.$GLOBALS['nl_b'];	// Nettopreis des Artikels
				$xml .= '          <ram:BasisQuantity unitCode="'.$pa['unitcode'].'">'.$this->fxcRound(1,4).'</ram:BasisQuantity>'.$GLOBALS['nl_b'];
				$xml .= '        </ram:NetPriceProductTradePrice>'.$GLOBALS['nl_b'];
				$xml .= '      </ram:SpecifiedLineTradeAgreement>'.$GLOBALS['nl_b'];
				// ......Gruppierung von Lieferangaben aus Positionsebene
				$xml .= '      <ram:SpecifiedLineTradeDelivery>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:BilledQuantity unitCode="'.$pa['unitcode'].'">'.$this->fxcRound($pa['amount'],4).'</ram:BilledQuantity>'.$GLOBALS['nl_b'];	// In Rechnung gestellte Menge + Code der Maßeinheit der in Rechnung gestellten Menge [fxcUnitCode]
				$xml .= '      </ram:SpecifiedLineTradeDelivery>'.$GLOBALS['nl_b'];
				// ......Gruppierung von Angaben zur Abrechnung auf Positionsebene / Umsatzsteuerinformation pro Position
				$xml .= '      <ram:SpecifiedLineTradeSettlement>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:ApplicableTradeTax>'.$GLOBALS['nl_b'];
				$xml .= '          <ram:TypeCode>VAT</ram:TypeCode>'.$GLOBALS['nl_b'];															// Steuerart - fester Wert "VAT"
				$xml .= '          <ram:CategoryCode>'.$pa['vcat'].'</ram:CategoryCode>'.$GLOBALS['nl_b'];										// Steuerkategorie: S oder Z
				$xml .= '          <ram:RateApplicablePercent>'.$this->fxcRound($pa['pvat'],2).'</ram:RateApplicablePercent>'.$GLOBALS['nl_b'];	// Umsatzsteuersatz
				$xml .= '        </ram:ApplicableTradeTax>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:SpecifiedTradeSettlementLineMonetarySummation>'.$GLOBALS['nl_b'];
				$xml .= '          <ram:LineTotalAmount>'.$this->fxcRound($pa['tnet'],2).'</ram:LineTotalAmount>'.$GLOBALS['nl_b'];				// Nettobetrag der Rechnungsposition
				$xml .= '        </ram:SpecifiedTradeSettlementLineMonetarySummation>'.$GLOBALS['nl_b'];
				$xml .= '      </ram:SpecifiedLineTradeSettlement>'.$GLOBALS['nl_b'];

				$xml .= '    </ram:IncludedSupplyChainTradeLineItem>'.$GLOBALS['nl_b'];
			}
		}

		// ApplicableHeaderTradeAgreement: Seller + Buyer
		$xml .= '    <ram:ApplicableHeaderTradeAgreement>'.$GLOBALS['nl_b'];
		// ...SellerTradeParty = Verkäufer
		$xml .= '      <ram:SellerTradeParty>'.$GLOBALS['nl_b'];
		$xml .= '        <ram:ID>'.$this->_fxa['client']['cid'].'</ram:ID>'.$GLOBALS['nl_b'];																// Kennung des Verkäufers
		if(strlen($this->_fxa['client']['gis']) && strlen($this->_fxa['client']['gid']))
			$xml .= '        <ram:GlobalID schemeID="'.$this->_fxa['client']['gis'].'">'.$this->_fxa['client']['gid'].'</ram:GlobalID>'.$GLOBALS['nl_b'];	// (Optional) Globaler Identifier des Verkäufers [fxcSchemeID]:  0088=EAN Location Code  (Sheet "EAS")
		$xml .= '        <ram:Name>'.$this->fxcText($this->_fxa['client']['nam']).'</ram:Name>'.$GLOBALS['nl_b'];											// Name des Verkäufers
		if(strlen($this->_fxa['client']['cnt']) || strlen($this->_fxa['client']['tel']) || strlen($this->_fxa['client']['eml']))
		{
			$xml .= '        <ram:DefinedTradeContact>'.$GLOBALS['nl_b'];
			if(strlen($this->_fxa['client']['cnt']))
				$xml .= '          <ram:PersonName>'.$this->fxcText($this->_fxa['client']['cnt']).'</ram:PersonName>'.$GLOBALS['nl_b'];						// (Optional) Kontaktstelle des Verkäufers, z.B. Name der Kontaktperson bei Firma
			if(strlen($this->_fxa['client']['tel']))
			{
				$xml .= '          <ram:TelephoneUniversalCommunication>'.$GLOBALS['nl_b'];
				$xml .= '            <ram:CompleteNumber>'.$this->_fxa['client']['tel'].'</ram:CompleteNumber>'.$GLOBALS['nl_b'];							// (Optional) Telefonnumer des Verkäufers
				$xml .= '          </ram:TelephoneUniversalCommunication>'.$GLOBALS['nl_b'];
			}
			if(strlen($this->_fxa['client']['eml']))
			{
				$xml .= '          <ram:EmailURIUniversalCommunication>'.$GLOBALS['nl_b'];
				$xml .= '            <ram:URIID>'.$this->_fxa['client']['eml'].'</ram:URIID>'.$GLOBALS['nl_b'];												// (Optional) E-Mail-Adresse des Verkäufers
				$xml .= '          </ram:EmailURIUniversalCommunication>'.$GLOBALS['nl_b'];
			}
			$xml .= '        </ram:DefinedTradeContact>'.$GLOBALS['nl_b'];
		}
		$xml .= '        <ram:PostalTradeAddress>'.$GLOBALS['nl_b'];
		$xml .= '          <ram:PostcodeCode>'.$this->_fxa['client']['pst'].'</ram:PostcodeCode>'.$GLOBALS['nl_b'];											// Postleitzahl der Verkäuferanschrift
		$xml .= '          <ram:LineOne>'.$this->fxcText($this->_fxa['client']['ln1']).'</ram:LineOne>'.$GLOBALS['nl_b'];									// Zeile 1 der Verkäuferanschrif (Straßenname und Hausnummer)
		if(strlen($this->_fxa['client']['ln2']))
			$xml .= '          <ram:LineTwo>'.$this->fxcText($this->_fxa['client']['ln2']).'</ram:LineTwo>'.$GLOBALS['nl_b'];								// (Optional) Zeile 2 der Verkäuferanschrif (Zusatz 1)
		if(strlen($this->_fxa['client']['ln3']))
			$xml .= '          <ram:LineThree>'.$this->fxcText($this->_fxa['client']['ln3']).'</ram:LineThree>'.$GLOBALS['nl_b'];							// (Optional) Zeile 3 der Verkäuferanschrif (Zusatz 2)
		$xml .= '          <ram:CityName>'.$this->fxcText($this->_fxa['client']['cty']).'</ram:CityName>'.$GLOBALS['nl_b'];									// Stadt der Verkäuferanschrift
		$xml .= '          <ram:CountryID>'.$this->_fxa['client']['ctr'].'</ram:CountryID>'.$GLOBALS['nl_b'];												// Ländercode der Verkäuferanschrift [fxcCountryID]: DE=Germany  (Sheet "Country")
		if(strlen($this->_fxa['client']['sdn']))
			$xml .= '          <ram:CountrySubDivisionName>'.$this->fxcText($this->_fxa['client']['sdn']).'</ram:CountrySubDivisionName>'.$GLOBALS['nl_b'];	// (Optional) Region oder Bundesland der Verkäuferanschrift
		$xml .= '        </ram:PostalTradeAddress>'.$GLOBALS['nl_b'];
		// ......Detailinformationen zu Steuerangaben des Verkäufers
		$xml .= '        <ram:SpecifiedTaxRegistration>'.$GLOBALS['nl_b'];
		$xml .= '          <ram:ID schemeID="FC">'.$this->_fxa['client']['tfc'].'</ram:ID>'.$GLOBALS['nl_b'];												// Steuernummer + Art der Steuernummer [fxcSchemeIDVAT] FC=Fiscal number / Steuernummer  (Sheet "VAT ID")
		$xml .= '        </ram:SpecifiedTaxRegistration>'.$GLOBALS['nl_b'];
		$xml .= '        <ram:SpecifiedTaxRegistration>'.$GLOBALS['nl_b'];
		$xml .= '          <ram:ID schemeID="VA">'.$this->_fxa['client']['tva'].'</ram:ID>'.$GLOBALS['nl_b'];												// Steuernummer + Art der Steuernummer [fxcSchemeIDVAT] VA: Value added tax / USt.-Id.-Nr.  (Sheet "VAT ID")
		$xml .= '        </ram:SpecifiedTaxRegistration>'.$GLOBALS['nl_b'];
		$xml .= '      </ram:SellerTradeParty>'.$GLOBALS['nl_b'];

		// ...BuyerTradeParty = Käufer/Leistungsempfänger
		$xml .= '      <ram:BuyerTradeParty>'.$GLOBALS['nl_b'];
		$xml .= '        <ram:ID>'.$this->_fxa['customer']['cid'].'</ram:ID>'.$GLOBALS['nl_b'];																// Kennung des Käufers
		$xml .= '        <ram:Name>'.$this->fxcText($this->_fxa['customer']['nam']).'</ram:Name>'.$GLOBALS['nl_b'];											// Name des Käufers
		if(strlen($this->_fxa['customer']['cnt']) || strlen($this->_fxa['client']['customer']) || strlen($this->_fxa['customer']['eml']))
		{
			$xml .= '        <ram:DefinedTradeContact>'.$GLOBALS['nl_b'];
			if(strlen($this->_fxa['customer']['cnt']))
				$xml .= '          <ram:PersonName>'.$this->fxcText($this->_fxa['customer']['cnt']).'</ram:PersonName>'.$GLOBALS['nl_b'];					// (Optional) Kontaktstelle des Käufers, z.B. Name der Kontaktperson bei Firma
			if(strlen($this->_fxa['customer']['tel']))
			{
				$xml .= '          <ram:TelephoneUniversalCommunication>'.$GLOBALS['nl_b'];
				$xml .= '            <ram:CompleteNumber>'.$this->_fxa['customer']['tel'].'</ram:CompleteNumber>'.$GLOBALS['nl_b'];							// (Optional) Telefonnumer des Käufers
				$xml .= '          </ram:TelephoneUniversalCommunication>'.$GLOBALS['nl_b'];
			}
			if(strlen($this->_fxa['customer']['eml']))
			{
				$xml .= '          <ram:EmailURIUniversalCommunication>'.$GLOBALS['nl_b'];
				$xml .= '            <ram:URIID>'.$this->_fxa['customer']['eml'].'</ram:URIID>'.$GLOBALS['nl_b'];											// (Optional) E-Mail-Adresse des Käufers
				$xml .= '          </ram:EmailURIUniversalCommunication>'.$GLOBALS['nl_b'];
			}
			$xml .= '        </ram:DefinedTradeContact>'.$GLOBALS['nl_b'];
		}
		$xml .= '        <ram:PostalTradeAddress>'.$GLOBALS['nl_b'];
		$xml .= '          <ram:PostcodeCode>'.$this->_fxa['customer']['pst'].'</ram:PostcodeCode>'.$GLOBALS['nl_b'];										// Postleitzahl der Käuferanschrift
		$xml .= '          <ram:LineOne>'.$this->fxcText($this->_fxa['customer']['ln1']).'</ram:LineOne>'.$GLOBALS['nl_b'];									// Zeile 1 der Käuferanschrif (Straßenname und Hausnummer)
		if(strlen($this->_fxa['customer']['ln2']))
			$xml .= '          <ram:LineTwo>'.$this->fxcText($this->_fxa['customer']['ln2']).'</ram:LineTwo>'.$GLOBALS['nl_b'];								// (Optional) Zeile 2 der Käuferanschrif (Zusatz 1)
		if(strlen($this->_fxa['customer']['ln3']))
			$xml .= '          <ram:LineThree>'.$this->fxcText($this->_fxa['customer']['ln3']).'</ram:LineThree>'.$GLOBALS['nl_b'];							// (Optional) Zeile 2 der Käuferanschrif (Zusatz 1)
		$xml .= '          <ram:CityName>'.$this->fxcText($this->_fxa['customer']['cty']).'</ram:CityName>'.$GLOBALS['nl_b'];								// Stadt der Käuferanschrift
		$xml .= '          <ram:CountryID>'.$this->_fxa['customer']['ctr'].'</ram:CountryID>'.$GLOBALS['nl_b'];												// Ländercode der Käuferanschrift [fxcCountryID]: DE=Germany  (Sheet "Country")
		$xml .= '        </ram:PostalTradeAddress>'.$GLOBALS['nl_b'];
		$xml .= '      </ram:BuyerTradeParty>'.$GLOBALS['nl_b'];

		// ...ContractReferencedDocument = Vertragsnummer
		if(strlen($this->_fxa['contract']))
		{
			$xml .= '      <ram:ContractReferencedDocument>'.$GLOBALS['nl_b'];
			$xml .= '        <ram:IssuerAssignedID>'.$this->fxcText($this->_fxa['contract']).'</ram:IssuerAssignedID>'.$GLOBALS['nl_b'];
			$xml .= '      </ram:ContractReferencedDocument>'.$GLOBALS['nl_b'];
		}

		// ...SellerOrderReferencedDocument = Bestellnummer
		if(strlen($this->_fxa['sorder']))
		{
			$xml .= '      <ram:SellerOrderReferencedDocument>'.$GLOBALS['nl_b'];
			$xml .= '        <ram:IssuerAssignedID>'.$this->fxcText($this->_fxa['sorder']).'</ram:IssuerAssignedID>'.$GLOBALS['nl_b'];
			$xml .= '      </ram:SellerOrderReferencedDocument>'.$GLOBALS['nl_b'];
		}

		// ...BuyerOrderReferencedDocument = Bestellnummer
		if(strlen($this->_fxa['border']))
		{
			$xml .= '      <ram:BuyerOrderReferencedDocument>'.$GLOBALS['nl_b'];
			$xml .= '        <ram:IssuerAssignedID>'.$this->fxcText($this->_fxa['border']).'</ram:IssuerAssignedID>'.$GLOBALS['nl_b'];
			$xml .= '      </ram:BuyerOrderReferencedDocument>'.$GLOBALS['nl_b'];
		}

		// ...SpecifiedProcuringProject = Projektangaben
		if(fxIsArray($this->_fxa['project']))
		{
			$xml .= '      <ram:SpecifiedProcuringProject>'.$GLOBALS['nl_b'];
			$xml .= '        <ram:ID>'.$this->_fxa['project']['id'].'</ram:ID>'.$GLOBALS['nl_b'];
			$xml .= '        <ram:Name>'.$this->fxcText($this->_fxa['project']['name']).'</ram:Name>'.$GLOBALS['nl_b'];
			$xml .= '      </ram:SpecifiedProcuringProject>'.$GLOBALS['nl_b'];
		}

		$xml .= '    </ram:ApplicableHeaderTradeAgreement>'.$GLOBALS['nl_b'];

		// ApplicableHeaderTradeDelivery / Lieferdatum (optional / obligatorisch falls es vom Rechnungsdatum abweicht)
		$xml .= '    <ram:ApplicableHeaderTradeDelivery>'.$GLOBALS['nl_b'];
		$xml .= '      <ram:ActualDeliverySupplyChainEvent>'.$GLOBALS['nl_b'];
		$xml .= '        <ram:OccurrenceDateTime>'.$GLOBALS['nl_b'];
		$xml .= '          <udt:DateTimeString format="102">'.$this->_fxa['ddate'].'</udt:DateTimeString>'.$GLOBALS['nl_b'];	// 102=CCYYMMDD  (Sheet "Date")
		$xml .= '        </ram:OccurrenceDateTime>'.$GLOBALS['nl_b'];
		$xml .= '      </ram:ActualDeliverySupplyChainEvent>'.$GLOBALS['nl_b'];
		$xml .= '    </ram:ApplicableHeaderTradeDelivery>'.$GLOBALS['nl_b'];

		// ApplicableHeaderTradeSettlement
		$xml .= '    <ram:ApplicableHeaderTradeSettlement>'.$GLOBALS['nl_b'];
		// ...Verwendungszweck: 872=Rechung
		$xml .= '      <ram:PaymentReference>'.meldung(872,false).': '.$this->fxcText($this->_fxa['number']).'</ram:PaymentReference>'.$GLOBALS['nl_b'];
		$xml .= '      <ram:InvoiceCurrencyCode>'.$this->_fxa['currency'].'</ram:InvoiceCurrencyCode>'.$GLOBALS['nl_b'];	// Sheet "Currency"
		// PaymentMeans / Zahlungsmittel
		if(sizeof($this->_fxa['payments']))
		{
			if($this->_lid == 1)	// Deutsch
				$inf='SEPA Banküberweisung';
			else
				$inf=$this->fxcPaymentType(58);

			$xml .= '      <ram:SpecifiedTradeSettlementPaymentMeans>'.$GLOBALS['nl_b'];
			$xml .= '        <ram:TypeCode>58</ram:TypeCode>'.$GLOBALS['nl_b'];	// 58: SEPA credit transfer [fxcPaymentType]   (Sheet "Payment")
			$xml .= '        <ram:Information>'.$inf.'</ram:Information>'.$GLOBALS['nl_b'];
			$xml .= '        <ram:PayeePartyCreditorFinancialAccount>'.$GLOBALS['nl_b'];
			$xml .= '          <ram:IBANID>'.$this->_fxa['payments']['iban'].'</ram:IBANID>'.$GLOBALS['nl_b'];
			if(strlen($this->_fxa['payments']['aname']))
			$xml .= '          <ram:AccountName>'.$this->fxcText($this->_fxa['payments']['aname']).'</ram:AccountName>'.$GLOBALS['nl_b'];
			$xml .= '        </ram:PayeePartyCreditorFinancialAccount>'.$GLOBALS['nl_b'];
			if(strlen($this->_fxa['payments']['bic']))
			{
				$xml .= '        <ram:PayeeSpecifiedCreditorFinancialInstitution>'.$GLOBALS['nl_b'];
				$xml .= '          <ram:BICID>'.$this->_fxa['payments']['bic'].'</ram:BICID>'.$GLOBALS['nl_b'];
				$xml .= '        </ram:PayeeSpecifiedCreditorFinancialInstitution>'.$GLOBALS['nl_b'];
			}
			$xml .= '      </ram:SpecifiedTradeSettlementPaymentMeans>'.$GLOBALS['nl_b'];
		}
		// ...Umsatzsteueraufschlüsselung(en)
		if(sizeof($this->_fxa['vats']))
		{
			foreach($this->_fxa['vats'] as $vr => $va)
			{
				$xml .= '      <ram:ApplicableTradeTax>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:CalculatedAmount>'.$this->fxcRound($va['tvat'],2).'</ram:CalculatedAmount>'.$GLOBALS['nl_b'];	// Umsatzsteuerbetrag
				$xml .= '        <ram:TypeCode>VAT</ram:TypeCode>'.$GLOBALS['nl_b'];												// Steuerart - fester Wert "VAT"
				$xml .= '        <ram:BasisAmount>'.$this->fxcRound($va['tnet'],4).'</ram:BasisAmount>'.$GLOBALS['nl_b'];			// Nettobetrag der Rechnungsposition
				$xml .= '        <ram:CategoryCode>'.$va['vcat'].'</ram:CategoryCode>'.$GLOBALS['nl_b'];							// Umsatzsteuerkategorie: S=Standard rate
				$xml .= '        <ram:RateApplicablePercent>'.$vr.'</ram:RateApplicablePercent>'.$GLOBALS['nl_b'];					// Umsatzsteuersatz
				$xml .= '      </ram:ApplicableTradeTax>'.$GLOBALS['nl_b'];
			}
		}

		// ...BillingSpecifiedPeriod / Abrechnungszeitraum
		if(sizeof($this->_fxa['timespan']))
		{
			$xml .= '      <ram:BillingSpecifiedPeriod>'.$GLOBALS['nl_b'];
			// Start
			if(isset($this->_fxa['timespan']['start']))
			{
				$xml .= '        <ram:StartDateTime>'.$GLOBALS['nl_b'];
				$xml .= '          <udt:DateTimeString format="102">'.$this->_fxa['timespan']['start'].'</udt:DateTimeString>'.$GLOBALS['nl_b'];
				$xml .= '        </ram:StartDateTime>'.$GLOBALS['nl_b'];
			}
			// End
			if(isset($this->_fxa['timespan']['end']))
			{
				$xml .= '        <ram:EndDateTime>'.$GLOBALS['nl_b'];
				$xml .= '          <udt:DateTimeString format="102">'.$this->_fxa['timespan']['end'].'</udt:DateTimeString>'.$GLOBALS['nl_b'];
				$xml .= '        </ram:EndDateTime>'.$GLOBALS['nl_b'];
			}
			$xml .= '      </ram:BillingSpecifiedPeriod>'.$GLOBALS['nl_b'];
		}

		// ......Gruppierung von Angaben zu Zuschlägen/Abschlägen auf Positionsebene
		if(sizeof($this->_fxa['discounts']))
		{
			foreach($this->_fxa['discounts'] as $pa)
			{
				$xml .= '      <ram:SpecifiedTradeAllowanceCharge>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:ChargeIndicator>'.$GLOBALS['nl_b'];
				$xml .= '          <udt:Indicator>false</udt:Indicator>'.$GLOBALS['nl_b'];														// false=Nachlass
				$xml .= '        </ram:ChargeIndicator>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:BasisAmount>'.$this->fxcRound(-1.0*$pa['unet'],4).'</ram:BasisAmount>'.$GLOBALS['nl_b'];					// Grundbetrag des Abschlags auf Ebene der Rechnungsposition
				$xml .= '        <ram:ActualAmount>'.$this->fxcRound(-1.0*$pa['unet'],4).'</ram:ActualAmount>'.$GLOBALS['nl_b'];				// Betrag des Abschlags auf Ebene der Rechnungsposition
				$xml .= '        <ram:ReasonCode>100</ram:ReasonCode>'.$GLOBALS['nl_b'];														// 100=Special rebate/Sonderrabatt
				$xml .= '        <ram:Reason>'.$this->fxcText($pa['name']).'</ram:Reason>'.$GLOBALS['nl_b'];
				$xml .= '        <ram:CategoryTradeTax>'.$GLOBALS['nl_b'];
				$xml .= '          <ram:TypeCode>VAT</ram:TypeCode>'.$GLOBALS['nl_b'];															// Steuerart - fester Wert "VAT"
				$xml .= '          <ram:CategoryCode>'.$pa['vcat'].'</ram:CategoryCode>'.$GLOBALS['nl_b'];										// Steuerkategorie: S oder Z
				$xml .= '          <ram:RateApplicablePercent>'.$this->fxcRound($pa['pvat'],2).'</ram:RateApplicablePercent>'.$GLOBALS['nl_b'];	// Umsatzsteuersatz
				$xml .= '        </ram:CategoryTradeTax>'.$GLOBALS['nl_b'];
				$xml .= '      </ram:SpecifiedTradeAllowanceCharge>'.$GLOBALS['nl_b'];
			}
		}

		// SpecifiedTradePaymentTerms / Zahlungsangaben / Skonto / Fälligkeitsdatum
		if(strlen($this->_fxa['terms']) || strlen($this->_fxa['pdate']))
		{
			$xml .= '      <ram:SpecifiedTradePaymentTerms>'.$GLOBALS['nl_b'];
			if(strlen($this->_fxa['terms']))	// Zahlungsangaben / Skonto
				$xml .= '        <ram:Description>'.$this->fxcText($this->_fxa['terms']).'</ram:Description>'.$GLOBALS['nl_b'];
			if(strlen($this->_fxa['pdate']))	// Falligkeitsdatum
			{
				$xml .= '        <ram:DueDateDateTime>'.$GLOBALS['nl_b'];
				$xml .= '          <udt:DateTimeString format="102">'.$this->_fxa['pdate'].'</udt:DateTimeString>'.$GLOBALS['nl_b'];
				$xml .= '        </ram:DueDateDateTime>'.$GLOBALS['nl_b'];
			}
			$xml .= '      </ram:SpecifiedTradePaymentTerms>'.$GLOBALS['nl_b'];
		}

		// SpecifiedTradeSettlementHeaderMonetarySummation / Rechnungssumme
		$xml .= '      <ram:SpecifiedTradeSettlementHeaderMonetarySummation>'.$GLOBALS['nl_b'];
		$xml .= '        <ram:LineTotalAmount>'.$this->fxcRound($this->_fxa['sum']['pos'],2).'</ram:LineTotalAmount>'.$GLOBALS['nl_b'];											// 473.00	Gesamtnettowert vor Steuern
		if($this->_fxa['sum']['chg'] != 0.0)
			$xml .= '        <ram:ChargeTotalAmount>'.$this->fxcRound($this->_fxa['sum']['chg'],2).'</ram:ChargeTotalAmount>'.$GLOBALS['nl_b'];									// 0.00		Summe der Abgaben auf Dokumentebene (optional)
		if($this->_fxa['sum']['all'] != 0.0)
			$xml .= '        <ram:AllowanceTotalAmount>'.$this->fxcRound($this->_fxa['sum']['all'],2).'</ram:AllowanceTotalAmount>'.$GLOBALS['nl_b'];							// 0.00		Summe der Freibeträge auf Dokumentebene (optional)
		$xml .= '        <ram:TaxBasisTotalAmount>'.$this->fxcRound($this->_fxa['sum']['net'],2).'</ram:TaxBasisTotalAmount>'.$GLOBALS['nl_b'];									// 473.00	Betrag ohne Steuern
		$xml .= '        <ram:TaxTotalAmount currencyID="'.$this->_fxa['currency'].'">'.$this->fxcRound($this->_fxa['sum']['vat'],2).'</ram:TaxTotalAmount>'.$GLOBALS['nl_b'];	// 56.87	Gesamtbetrag der Steuern
		$xml .= '        <ram:GrandTotalAmount>'.$this->fxcRound($this->_fxa['sum']['grs'],2).'</ram:GrandTotalAmount>'.$GLOBALS['nl_b'];										// 529.87	Gesamtbruttobetrag inkl. Steuern
		$xml .= '        <ram:TotalPrepaidAmount>'.$this->fxcRound($this->_fxa['sum']['pre'],2).'</ram:TotalPrepaidAmount>'.$GLOBALS['nl_b'];									// 0.00		Vorauskasse
		$xml .= '        <ram:DuePayableAmount>'.$this->fxcRound($this->_fxa['sum']['pay'],2).'</ram:DuePayableAmount>'.$GLOBALS['nl_b'];										// 529.87	Zu zahlender Nettobetrag
		$xml .= '      </ram:SpecifiedTradeSettlementHeaderMonetarySummation>'.$GLOBALS['nl_b'];
		$xml .= '    </ram:ApplicableHeaderTradeSettlement>'.$GLOBALS['nl_b'];
		$xml .= '  </rsm:SupplyChainTradeTransaction>'.$GLOBALS['nl_b'];
		$xml .= '</rsm:CrossIndustryInvoice>'.$GLOBALS['nl_b'];

		// Save XML?
		if(is_string($save) && strlen($save))
			$se=fxSaveBin($save, $xml);

		return $xml;
	}

	public function fxcInvoiceType($code=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$code=(int)$code;

		$a=array(
			71=>'Request for payment',
			80=>'Debit note related to goods or services',
			81=>'Credit note related to goods or services',
			82=>'Metered services invoice',
			83=>'Credit note related to financial adjustments',
			84=>'Debit note related to financial adjustments',
			102=>'Tax notification',
			130=>'Invoicing data sheet',
			202=>'Direct payment valuation',
			203=>'Provisional payment valuation',
			204=>'Payment valuation',
			211=>'Interim application for payment',
			218=>'Final payment request based on completion of work',
			219=>'Payment request for completed units',
			261=>'Self billed credit note',
			262=>'Consolidated credit note - goods and services',
			295=>'Price variation invoice',
			296=>'Credit note for price variation',
			308=>'Delcredere credit note',
			325=>'Proforma invoice',
			326=>'Partial invoice',
			331=>'Commercial invoice which includes a packing list',
			380=>'Commercial invoice',
			381=>'Credit note',
			382=>'Commission note',
			383=>'Debit note',
			384=>'Corrected invoice',
			385=>'Consolidated invoice',
			386=>'Prepayment invoice',
			387=>'Hire invoice',
			388=>'Tax invoice',
			389=>'Self-billed invoice',
			390=>'Delcredere invoice',
			393=>'Factored invoice',
			394=>'Lease invoice',
			395=>'Consignment invoice',
			396=>'Factored credit note',
			420=>'Optical Character Reading (OCR) payment credit note',
			456=>'Debit advice',
			457=>'Reversal of debit',
			458=>'Reversal of credit',
			527=>'Self billed debit note',
			532=>'Forwarder\'s credit note',
			553=>'Forwarder\'s invoice discrepancy report',
			575=>'Insurer\'s invoice',
			623=>'Forwarder\'s invoice',
			633=>'Port charges documents',
			751=>'Invoice information for accounting purposes',
			780=>'Freight invoice',
			817=>'Claim notification',
			870=>'Consular invoice',
			875=>'Partial construction invoice',
			876=>'Partial final construction invoice',
			877=>'Final construction invoice',
			935=>'Customs invoice'
		);

		if(!$code)
			return $a;
		else if(isset($a[$code]))
			return $a[$code];
		else
			return '['.$code.' !]';
	}

	public function fxcSubjectCode($code='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$code=trim((string)$code);

		$a=array(
			'AAA'=>'Goods item description',
			'AAB'=>'Payment term',
			'AAC'=>'Dangerous goods additional information',
			'AAD'=>'Dangerous goods technical name',
			'AAE'=>'Acknowledgement description',
			'AAF'=>'Rate additional information',
			'AAG'=>'Party instructions',
			'AAI'=>'General information',
			'AAJ'=>'Additional conditions of sale/purchase',
			'AAK'=>'Price conditions',
			'AAL'=>'Goods dimensions in characters',
			'AAM'=>'Equipment re-usage restrictions',
			'AAN'=>'Handling restriction',
			'AAO'=>'Error description (free text)',
			'AAP'=>'Response (free text)',
			'AAQ'=>'Package content\'s description',
			'AAR'=>'Terms of delivery',
			'AAS'=>'Bill of lading remarks',
			'AAT'=>'Mode of settlement information',
			'AAU'=>'Consignment invoice information',
			'AAV'=>'Clearance invoice information',
			'AAW'=>'Letter of credit information',
			'AAX'=>'License information',
			'AAY'=>'Certification statements',
			'AAZ'=>'Additional export information',
			'ABA'=>'Tariff statements',
			'ABB'=>'Medical history',
			'ABC'=>'Conditions of sale or purchase',
			'ABD'=>'Contract document type',
			'ABE'=>'Additional terms and/or conditions (documentary credit)',
			'ABF'=>'Instructions or information about standby documentary',
			'ABG'=>'Instructions or information about partial shipment(s)',
			'ABH'=>'Instructions or information about transhipment(s)',
			'ABI'=>'Additional handling instructions documentary credit',
			'ABJ'=>'Domestic routing information',
			'ABK'=>'Chargeable category of equipment',
			'ABL'=>'Government information',
			'ABM'=>'Onward routing information',
			'ABN'=>'Accounting information',
			'ABO'=>'Discrepancy information',
			'ABP'=>'Confirmation instructions',
			'ABQ'=>'Method of issuance',
			'ABR'=>'Documents delivery instructions',
			'ABS'=>'Additional conditions',
			'ABT'=>'Information/instructions about additional amounts covered',
			'ABU'=>'Deferred payment termed additional',
			'ABV'=>'Acceptance terms additional',
			'ABW'=>'Negotiation terms additional',
			'ABX'=>'Document name and documentary requirements',
			'ABZ'=>'Instructions/information about revolving documentary credit',
			'ACA'=>'Documentary requirements',
			'ACB'=>'Additional information',
			'ACC'=>'Factor assignment clause',
			'ACD'=>'Reason',
			'ACE'=>'Dispute',
			'ACF'=>'Additional attribute information',
			'ACG'=>'Absence declaration',
			'ACH'=>'Aggregation statement',
			'ACI'=>'Compilation statement',
			'ACJ'=>'Definitional exception',
			'ACK'=>'Privacy statement',
			'ACL'=>'Quality statement',
			'ACM'=>'Statistical description',
			'ACN'=>'Statistical definition',
			'ACO'=>'Statistical name',
			'ACP'=>'Statistical title',
			'ACQ'=>'Off-dimension information',
			'ACR'=>'Unexpected stops information',
			'ACS'=>'Principles',
			'ACT'=>'Terms and definition',
			'ACU'=>'Segment name',
			'ACV'=>'Simple data element name',
			'ACW'=>'Scope',
			'ACX'=>'Message type name',
			'ACY'=>'Introduction',
			'ACZ'=>'Glossary',
			'ADA'=>'Functional definition',
			'ADB'=>'Examples',
			'ADC'=>'Cover page',
			'ADD'=>'Dependency (syntax) notes',
			'ADE'=>'Code value name',
			'ADF'=>'Code list name',
			'ADG'=>'Clarification of usage',
			'ADH'=>'Composite data element name',
			'ADI'=>'Field of application',
			'ADJ'=>'Type of assets and liabilities',
			'ADK'=>'Promotion information',
			'ADL'=>'Meter condition',
			'ADM'=>'Meter reading information',
			'ADN'=>'Type of transaction reason',
			'ADO'=>'Type of survey question',
			'ADP'=>'Carrier\'s agent counter information',
			'ADQ'=>'Description of work item on equipment',
			'ADR'=>'Message definition',
			'ADS'=>'Booked item information',
			'ADT'=>'Source of document',
			'ADU'=>'Note',
			'ADV'=>'Fixed part of segment clarification text',
			'ADW'=>'Characteristics of goods',
			'ADX'=>'Additional discharge instructions',
			'ADY'=>'Container stripping instructions',
			'ADZ'=>'CSC (Container Safety Convention) plate information',
			'AEA'=>'Cargo remarks',
			'AEB'=>'Temperature control instructions',
			'AEC'=>'Text refers to expected data',
			'AED'=>'Text refers to received data',
			'AEE'=>'Section clarification text',
			'AEF'=>'Information to the beneficiary',
			'AEG'=>'Information to the applicant',
			'AEH'=>'Instructions to the beneficiary',
			'AEI'=>'Instructions to the applicant',
			'AEJ'=>'Controlled atmosphere',
			'AEK'=>'Take off annotation',
			'AEL'=>'Price variation narrative',
			'AEM'=>'Documentary credit amendment instructions',
			'AEN'=>'Standard method narrative',
			'AEO'=>'Project narrative',
			'AEP'=>'Radioactive goods, additional information',
			'AEQ'=>'Bank-to-bank information',
			'AER'=>'Reimbursement instructions',
			'AES'=>'Reason for amending a message',
			'AET'=>'Instructions to the paying and/or accepting and/or',
			'AEU'=>'Interest instructions',
			'AEV'=>'Agent commission',
			'AEW'=>'Remitting bank instructions',
			'AEX'=>'Instructions to the collecting bank',
			'AEY'=>'Collection amount instructions',
			'AEZ'=>'Internal auditing information',
			'AFA'=>'Constraint',
			'AFB'=>'Comment',
			'AFC'=>'Semantic note',
			'AFD'=>'Help text',
			'AFE'=>'Legend',
			'AFF'=>'Batch code structure',
			'AFG'=>'Product application',
			'AFH'=>'Customer complaint',
			'AFI'=>'Probable cause of fault',
			'AFJ'=>'Defect description',
			'AFK'=>'Repair description',
			'AFL'=>'Review comments',
			'AFM'=>'Title',
			'AFN'=>'Description of amount',
			'AFO'=>'Responsibilities',
			'AFP'=>'Supplier',
			'AFQ'=>'Purchase region',
			'AFR'=>'Affiliation',
			'AFS'=>'Borrower',
			'AFT'=>'Line of business',
			'AFU'=>'Financial institution',
			'AFV'=>'Business founder',
			'AFW'=>'Business history',
			'AFX'=>'Banking arrangements',
			'AFY'=>'Business origin',
			'AFZ'=>'Brand names\' description',
			'AGA'=>'Business financing details',
			'AGB'=>'Competition',
			'AGC'=>'Construction process details',
			'AGD'=>'Construction specialty',
			'AGE'=>'Contract information',
			'AGF'=>'Corporate filing',
			'AGG'=>'Customer information',
			'AGH'=>'Copyright notice',
			'AGI'=>'Contingent debt',
			'AGJ'=>'Conviction details',
			'AGK'=>'Equipment',
			'AGL'=>'Workforce description',
			'AGM'=>'Exemption',
			'AGN'=>'Future plans',
			'AGO'=>'Interviewee conversation information',
			'AGP'=>'Intangible asset',
			'AGQ'=>'Inventory',
			'AGR'=>'Investment',
			'AGS'=>'Intercompany relations information',
			'AGT'=>'Joint venture',
			'AGU'=>'Loan',
			'AGV'=>'Long term debt',
			'AGW'=>'Location',
			'AGX'=>'Current legal structure',
			'AGY'=>'Marital contract',
			'AGZ'=>'Marketing activities',
			'AHA'=>'Merger',
			'AHB'=>'Marketable securities',
			'AHC'=>'Business debt',
			'AHD'=>'Original legal structure',
			'AHE'=>'Employee sharing arrangements',
			'AHF'=>'Organization details',
			'AHG'=>'Public record details',
			'AHH'=>'Price range',
			'AHI'=>'Qualifications',
			'AHJ'=>'Registered activity',
			'AHK'=>'Criminal sentence',
			'AHL'=>'Sales method',
			'AHM'=>'Educational institution information',
			'AHN'=>'Status details',
			'AHO'=>'Sales',
			'AHP'=>'Spouse information',
			'AHQ'=>'Educational degree information',
			'AHR'=>'Shareholding information',
			'AHS'=>'Sales territory',
			'AHT'=>'Accountant\'s comments',
			'AHU'=>'Exemption law location',
			'AHV'=>'Share classifications',
			'AHW'=>'Forecast',
			'AHX'=>'Event location',
			'AHY'=>'Facility occupancy',
			'AHZ'=>'Import and export details',
			'AIA'=>'Additional facility information',
			'AIB'=>'Inventory value',
			'AIC'=>'Education',
			'AID'=>'Event',
			'AIE'=>'Agent',
			'AIF'=>'Domestically agreed financial statement details',
			'AIG'=>'Other current asset description',
			'AIH'=>'Other current liability description',
			'AII'=>'Former business activity',
			'AIJ'=>'Trade name use',
			'AIK'=>'Signing authority',
			'AIL'=>'Guarantee',
			'AIM'=>'Holding company operation',
			'AIN'=>'Consignment routing',
			'AIO'=>'Letter of protest',
			'AIP'=>'Question',
			'AIQ'=>'Party information',
			'AIR'=>'Area boundaries description',
			'AIS'=>'Advertisement information',
			'AIT'=>'Financial statement details',
			'AIU'=>'Access instructions',
			'AIV'=>'Liquidity',
			'AIW'=>'Credit line',
			'AIX'=>'Warranty terms',
			'AIY'=>'Division description',
			'AIZ'=>'Reporting instruction',
			'AJA'=>'Examination result',
			'AJB'=>'Laboratory result',
			'ALC'=>'Allowance/charge information',
			'ALD'=>'X-ray result',
			'ALE'=>'Pathology result',
			'ALF'=>'Intervention description',
			'ALG'=>'Summary of admittance',
			'ALH'=>'Medical treatment course detail',
			'ALI'=>'Prognosis',
			'ALJ'=>'Instruction to patient',
			'ALK'=>'Instruction to physician',
			'ALL'=>'All documents',
			'ALM'=>'Medicine treatment',
			'ALN'=>'Medicine dosage and administration',
			'ALO'=>'Availability of patient',
			'ALP'=>'Reason for service request',
			'ALQ'=>'Purpose of service',
			'ARR'=>'Arrival conditions',
			'ARS'=>'Service requester\'s comment',
			'AUT'=>'Authentication',
			'AUU'=>'Requested location description',
			'AUV'=>'Medicine administration condition',
			'AUW'=>'Patient information',
			'AUX'=>'Precautionary measure',
			'AUY'=>'Service characteristic',
			'AUZ'=>'Planned event comment',
			'AVA'=>'Expected delay comment',
			'AVB'=>'Transport requirements comment',
			'AVC'=>'Temporary approval condition',
			'AVD'=>'Customs Valuation Information',
			'AVE'=>'Value Added Tax (VAT) margin scheme',
			'AVF'=>'Maritime Declaration of Health',
			'BAG'=>'Passenger baggage information',
			'BAH'=>'Maritime Declaration of Health',
			'BAI'=>'Additional product information address',
			'BAJ'=>'Information to be printed on despatch advice',
			'BAK'=>'Missing goods remarks',
			'BAL'=>'Non-acceptance information',
			'BAM'=>'Returns information',
			'BAN'=>'Sub-line item information',
			'BAO'=>'Test information',
			'BAP'=>'External link',
			'BAQ'=>'VAT exemption reason',
			'BAR'=>'Processing Instructions',
			'BAS'=>'Relay Instructions',
			'BAT'=>'SIMA applicable',
			'BAU'=>'Appeals program code',
			'BAV'=>'SIMA subject',
			'BAW'=>'Surtax applicable',
			'BAX'=>'SIMA security bond',
			'BAY'=>'Surtax subject',
			'BAZ'=>'Safeguard applicable',
			'BBA'=>'Safeguard applicable',
			'BBB'=>'Safeguard subject',
			'BLC'=>'Transport contract document clause',
			'BLD'=>'Instruction to prepare the patient',
			'BLE'=>'Medicine treatment comment',
			'BLF'=>'Examination result comment',
			'BLG'=>'Service request comment',
			'BLH'=>'Prescription reason',
			'BLI'=>'Prescription comment',
			'BLJ'=>'Clinical investigation comment',
			'BLK'=>'Medicinal specification comment',
			'BLL'=>'Economic contribution comment',
			'BLM'=>'Status of a plan',
			'BLN'=>'Random sample test information',
			'BLO'=>'Period of time',
			'BLP'=>'Legislation',
			'BLQ'=>'Security measures requested',
			'BLR'=>'Transport contract document remark',
			'BLS'=>'Previous port of call security information',
			'BLT'=>'Security information',
			'BLU'=>'Waste information',
			'BLV'=>'B2C marketing information, short description',
			'BLW'=>'B2B marketing information, long description',
			'BLX'=>'B2C marketing information, long description',
			'BLY'=>'Product ingredients',
			'BLZ'=>'Location short name',
			'BMA'=>'Packaging material information',
			'BMB'=>'Filler material information',
			'BMC'=>'Ship-to-ship activity information',
			'BMD'=>'Package material description',
			'BME'=>'Consumer level package marking',
			'BMF'=>'SIMA measure in force',
			'BMG'=>'Pre-CARM',
			'BMH'=>'SIMA measure type',
			'CCI'=>'Customs clearance instructions',
			'CCJ'=>'Sub Type Code',
			'CCK'=>'SIMA information',
			'CCL'=>'Time limit end',
			'CCM'=>'Time limit start',
			'CCN'=>'Warehouse time limit',
			'CCO'=>'Value for duty information',
			'CEX'=>'Customs clearance instructions export',
			'CHG'=>'Change information',
			'CIP'=>'Customs clearance instruction import',
			'CLP'=>'Clearance place requested',
			'CLR'=>'Loading remarks',
			'COI'=>'Order information',
			'CUR'=>'Customer remarks',
			'CUS'=>'Customs declaration information',
			'DAR'=>'Damage remarks',
			'DCL'=>'Document issuer declaration',
			'DEL'=>'Delivery information',
			'DIN'=>'Delivery instructions',
			'DOC'=>'Documentation instructions',
			'DUT'=>'Duty declaration',
			'EUR'=>'Effective used routing',
			'FBC'=>'First block to be printed on the transport contract',
			'GBL'=>'Government bill of lading information',
			'GEN'=>'Entire transaction set',
			'GS7'=>'Further information concerning GGVS par. 7',
			'HAN'=>'Consignment handling instruction',
			'HAZ'=>'Hazard information',
			'ICN'=>'Consignment information for consignee',
			'IIN'=>'Insurance instructions',
			'IMI'=>'Invoice mailing instructions',
			'IND'=>'Commercial invoice item description',
			'INS'=>'Insurance information',
			'INV'=>'Invoice instruction',
			'IRP'=>'Information for railway purpose',
			'ITR'=>'Inland transport details',
			'ITS'=>'Testing instructions',
			'LAN'=>'Location Alias',
			'LIN'=>'Line item',
			'LOI'=>'Loading instruction',
			'MCO'=>'Miscellaneous charge order',
			'MDH'=>'Maritime Declaration of Health',
			'MKS'=>'Additional marks/numbers information',
			'ORI'=>'Order instruction',
			'OSI'=>'Other service information',
			'PAC'=>'Packing/marking information',
			'PAI'=>'Payment instructions information',
			'PAY'=>'Payables information',
			'PKG'=>'Packaging information',
			'PKT'=>'Packaging terms information',
			'PMD'=>'Payment detail/remittance information',
			'PMT'=>'Payment information',
			'PRD'=>'Product information',
			'PRF'=>'Price calculation formula',
			'PRI'=>'Priority information',
			'PUR'=>'Purchasing information',
			'QIN'=>'Quarantine instructions',
			'QQD'=>'Quality demands/requirements',
			'QUT'=>'Quotation instruction/information',
			'RAH'=>'Risk and handling information',
			'REG'=>'Regulatory information',
			'RET'=>'Return to origin information',
			'REV'=>'Receivables',
			'RQR'=>'Consignment route',
			'SAF'=>'Safety information',
			'SIC'=>'Consignment documentary instruction',
			'SIN'=>'Special instructions',
			'SLR'=>'Ship line requested',
			'SPA'=>'Special permission for transport, generally',
			'SPG'=>'Special permission concerning the goods to be transported',
			'SPH'=>'Special handling',
			'SPP'=>'Special permission concerning package',
			'SPT'=>'Special permission concerning transport means',
			'SRN'=>'Subsidiary risk number (IATA/DGR)',
			'SSR'=>'Special service request',
			'SUR'=>'Supplier remarks',
			'TCA'=>'Consignment tariff',
			'TDT'=>'Consignment transport',
			'TRA'=>'Transportation information',
			'TRR'=>'Requested tariff',
			'TXD'=>'Tax declaration',
			'WHI'=>'Warehouse instruction/information',
			'ZZZ'=>'Mutually defined'
		);

		if(!strlen($code))
			return $a;
		else if(isset($a[$code]))
			return $a[$code];
		else
			return '['.$code.' !]';
	}

	public function fxcSchemeID($code='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$code=substr('000'.(int)$code, -4);

		$a=array(
			'0002'=>'System Information et Repertoire des Entreprise et des Etablissements: SIRENE',
			'0003'=>'Codification Numerique des Etablissments Financiers En Belgique',
			'0004'=>'NBS/OSI NETWORK',
			'0005'=>'USA FED GOV OSI NETWORK',
			'0006'=>'USA DOD OSI NETWORK',
			'0007'=>'Organisationsnummer',
			'0008'=>'LE NUMERO NATIONAL',
			'0009'=>'SIRET-CODE',
			'0010'=>'Organizational Identifiers for Structured Names under ISO 9541 Part 2',
			'0011'=>'International Code Designator for the Identification of OSI-based, Amateur Radio Organizations, Network Objects and Application Services.',
			'0012'=>'European Computer Manufacturers Association: ECMA',
			'0013'=>'VSA FTP CODE (FTP = File Transfer Protocol)',
			'0014'=>'NIST/OSI Implememts\' Workshop',
			'0015'=>'Electronic Data Interchange: EDI',
			'0016'=>'EWOS Object Identifiers',
			'0017'=>'COMMON LANGUAGE',
			'0018'=>'SNA/OSI Network',
			'0019'=>'Air Transport Industry Services Communications Network',
			'0020'=>'European Laboratory for Particle Physics: CERN',
			'0021'=>'SOCIETY FOR WORLDWIDE INTERBANK FINANCIAL, TELECOMMUNICATION S.W.I.F.T.',
			'0022'=>'OSF Distributed Computing Object Identification',
			'0023'=>'Nordic University and Research Network: NORDUnet',
			'0024'=>'Digital Equipment Corporation: DEC',
			'0025'=>'OSI ASIA-OCEANIA WORKSHOP',
			'0026'=>'NATO ISO 6523 ICDE coding scheme',
			'0027'=>'Aeronautical Telecommunications Network (ATN)',
			'0028'=>'International Standard ISO 6523',
			'0029'=>'The All-Union Classifier of Enterprises and Organisations',
			'0030'=>'AT&T/OSI Network',
			'0031'=>'EDI Partner Identification Code',
			'0032'=>'Telecom Australia',
			'0033'=>'S G W OSI Internetwork',
			'0034'=>'Reuter Open Address Standard',
			'0035'=>'ISO 6523 - ICD',
			'0036'=>'TeleTrust Object Identifiers',
			'0037'=>'LY-tunnus',
			'0038'=>'The Australian GOSIP Network',
			'0039'=>'The OZ DOD OSI Network',
			'0040'=>'Unilever Group Companies',
			'0041'=>'Citicorp Global Information Network',
			'0042'=>'DBP Telekom Object Identifiers',
			'0043'=>'HydroNETT',
			'0044'=>'Thai Industrial Standards Institute (TISI)',
			'0045'=>'ICI Company Identification System',
			'0046'=>'FUNLOC',
			'0047'=>'BULL ODI/DSA/UNIX Network',
			'0048'=>'OSINZ',
			'0049'=>'Auckland Area Health',
			'0050'=>'Firmenich',
			'0051'=>'AGFA-DIS',
			'0052'=>'Society of Motion Picture and Television Engineers (SMPTE)',
			'0053'=>'Migros_Network M_NETOPZ',
			'0054'=>'ISO6523 - ICDPCR',
			'0055'=>'Energy Net',
			'0056'=>'Nokia Object Identifiers (NOI)',
			'0057'=>'Saint Gobain',
			'0058'=>'Siemens Corporate Network',
			'0059'=>'DANZNET',
			'0060'=>'Data Universal Numbering System (D-U-N-S Number)',
			'0061'=>'SOFFEX OSI',
			'0062'=>'KPN OVN',
			'0063'=>'ascomOSINet',
			'0064'=>'UTC: Uniforme Transport Code',
			'0065'=>'SOLVAY OSI CODING',
			'0066'=>'Roche Corporate Network',
			'0067'=>'ZellwegerOSINet',
			'0068'=>'Intel Corporation OSI',
			'0069'=>'SITA Object Identifier Tree',
			'0070'=>'DaimlerChrysler Corporate Network',
			'0071'=>'LEGO /OSI NETWORK',
			'0072'=>'NAVISTAR/OSI Network',
			'0073'=>'ICD Formatted ATM address',
			'0074'=>'ARINC',
			'0075'=>'Alcanet/Alcatel-Alsthom Corporate Network',
			'0076'=>'Sistema Italiano di Identificazione di ogetti gestito da UNINFO',
			'0077'=>'Sistema Italiano di Indirizzamento di Reti OSI Gestito da UNINFO',
			'0078'=>'Mitel terminal or switching equipment',
			'0079'=>'ATM Forum',
			'0080'=>'UK National Health Service Scheme, (EDIRA compliant)',
			'0081'=>'International NSAP',
			'0082'=>'Norwegian Telecommunications Authority\'s, NTA\'S, EDI, identifier scheme (EDIRA compliant)',
			'0083'=>'Advanced Telecommunications Modules Limited, Corporate Network',
			'0084'=>'Athens Chamber of Commerce & Industry Scheme (EDIRA compliant)',
			'0085'=>'Swiss Chambers of Commerce Scheme (EDIRA) compliant',
			'0086'=>'United States Council for International Business (USCIB) Scheme, (EDIRA compliant)',
			'0087'=>'National Federation of Chambers of Commerce & Industry of Belgium, Scheme (EDIRA compliant)',
			'0088'=>'EAN Location Code',
			'0089'=>'The Association of British Chambers of Commerce Ltd. Scheme, (EDIRA compliant)',
			'0090'=>'Internet IP addressing - ISO 6523 ICD encoding',
			'0091'=>'Cisco Sysytems / OSI Network',
			'0093'=>'Revenue Canada Business Number Registration (EDIRA compliant)',
			'0094'=>'DEUTSCHER INDUSTRIE- UND HANDELSTAG (DIHT) Scheme (EDIRA compliant)',
			'0095'=>'Hewlett - Packard Company Internal AM Network',
			'0096'=>'DANISH CHAMBER OF COMMERCE Scheme (EDIRA compliant)',
			'0097'=>'FTI - Ediforum Italia, (EDIRA compliant)',
			'0098'=>'CHAMBER OF COMMERCE TEL AVIV-JAFFA Scheme (EDIRA compliant)',
			'0099'=>'Siemens Supervisory Systems Network',
			'0100'=>'PNG_ICD Scheme',
			'0101'=>'South African Code Allocation',
			'0102'=>'HEAG',
			'0104'=>'BT - ICD Coding System',
			'0105'=>'Portuguese Chamber of Commerce and Industry Scheme (EDIRA compliant)',
			'0106'=>'Vereniging van Kamers van Koophandel en Fabrieken in Nederland (Association of Chambers of Commerce and Industry in the Netherlands), Scheme (EDIRA compliant)',
			'0107'=>'Association of Swedish Chambers of Commerce and Industry Scheme (EDIRA compliant)',
			'0108'=>'Australian Chambers of Commerce and Industry Scheme (EDIRA compliant)',
			'0109'=>'BellSouth ICD AESA (ATM End System Address)',
			'0110'=>'Bell Atlantic',
			'0111'=>'Object Identifiers',
			'0112'=>'ISO register for Standards producing Organizations',
			'0113'=>'OriginNet',
			'0114'=>'Check Point Software Technologies',
			'0115'=>'Pacific Bell Data Communications Network',
			'0116'=>'PSS Object Identifiers',
			'0117'=>'STENTOR-ICD CODING SYSTEM',
			'0118'=>'ATM-Network ZN\'96',
			'0119'=>'MCI / OSI Network',
			'0120'=>'Advantis',
			'0121'=>'Affable Software Data Interchange Codes',
			'0122'=>'BB-DATA GmbH',
			'0123'=>'BASF Company ATM-Network',
			'0124'=>'IOTA Identifiers for Organizations for Telecommunications Addressing using the ICD system format defined in ISO/IEC 8348',
			'0125'=>'Henkel Corporate Network (H-Net)',
			'0126'=>'GTE/OSI Network',
			'0127'=>'Dresdner Bank Corporate Network',
			'0128'=>'BCNR (Swiss Clearing Bank Number)',
			'0129'=>'BPI (Swiss Business Partner Identification) code',
			'0130'=>'Directorates of the European Commission',
			'0131'=>'Code for the Identification of National Organizations',
			'0132'=>'Certicom Object Identifiers',
			'0133'=>'TC68 OID',
			'0134'=>'Infonet Services Corporation',
			'0135'=>'SIA Object Identifiers',
			'0136'=>'Cable & Wireless Global ATM End-System Address Plan',
			'0137'=>'Global AESA scheme',
			'0138'=>'France Telecom ATM End System Address Plan',
			'0139'=>'Savvis Communications AESA:.',
			'0140'=>'Toshiba Organizations, Partners, And Suppliers\' (TOPAS) Code',
			'0141'=>'NATO Commercial and Government Entity system',
			'0142'=>'SECETI Object Identifiers',
			'0143'=>'EINESTEINet AG',
			'0144'=>'DoDAAC (Department of Defense Activity Address Code)',
			'0145'=>'DGCP (Direction Générale de la Comptabilité Publique)administrative accounting identification scheme',
			'0146'=>'DGI (Direction Générale des Impots) code',
			'0147'=>'Standard Company Code',
			'0148'=>'ITU (International Telecommunications Union)Data Network Identification Codes (DNIC)',
			'0149'=>'Global Business Identifier',
			'0150'=>'Madge Networks Ltd- ICD ATM Addressing Scheme',
			'0151'=>'Australian Business Number (ABN) Scheme',
			'0152'=>'Edira Scheme Identifier Code',
			'0153'=>'Concert Global Network Services ICD AESA',
			'0154'=>'Identification number of economic subjects: (ICO)',
			'0155'=>'Global Crossing AESA (ATM End System Address)',
			'0156'=>'AUNA',
			'0157'=>'ATM interconnection with the Dutch KPN Telecom',
			'0158'=>'Identification number of economic subject (ICO) Act on State Statistics of 29 November 2\'001, § 27',
			'0159'=>'ACTALIS Object Identifiers',
			'0160'=>'GTIN - Global Trade Item Number',
			'0161'=>'ECCMA Open Technical Directory',
			'0162'=>'CEN/ISSS Object Identifier Scheme',
			'0163'=>'US-EPA Facility Identifier',
			'0164'=>'TELUS Corporation',
			'0165'=>'FIEIE Object identifiers',
			'0166'=>'Swissguide Identifier Scheme',
			'0167'=>'Priority Telecom ATM End System Address Plan',
			'0168'=>'Vodafone Ireland OSI Addressing',
			'0169'=>'Swiss Federal Business Identification Number. Central Business names Index (zefix) Identification Number',
			'0170'=>'Teikoku Company Code',
			'0171'=>'Luxembourg CP & CPS (Certification Policy and Certification Practice Statement) Index',
			'0172'=>'Project Group “Lists of Properties” (PROLIST®)',
			'0173'=>'eCI@ss',
			'0174'=>'StepNexus',
			'0175'=>'Siemens AG',
			'0176'=>'Paradine GmbH',
			'0177'=>'Odette International Limited',
			'0178'=>'Route1 MobiNET',
			'0179'=>'Penango Object Identifiers',
			'0180'=>'Lithuanian military PKI',
			'0183'=>'Numéro d\'identification suisse des enterprises (IDE), Swiss Unique Business Identification Number (UIDB)',
			'0184'=>'DIGSTORG',
			'0185'=>'Perceval Object Code',
			'0186'=>'TrustPoint Object Identifiers',
			'0187'=>'Amazon Unique Identification Scheme',
			'0188'=>'Corporate Number of The Social Security and Tax Number System',
			'0189'=>'European Business Identifier (EBID)',
			'0190'=>'Organisatie Indentificatie Nummer (OIN)',
			'0191'=>'Company Code (Estonia)',
			'0192'=>'Organisasjonsnummer',
			'0193'=>'UBL.BE Party Identifier',
			'0194'=>'KOIOS Open Technical Dictionary',
			'0195'=>'Singapore Nationwide E-lnvoice Framework',
			'0196'=>'Icelandic identifier - Íslensk kennitala',
			'0197'=>'APPLiA Pl Standard',
			'0198'=>'ERSTORG',
			'0199'=>'Legal Entity Identifier (LEI)',
			'0200'=>'Legal entity code (Lithuania)',
			'0201'=>'Codice Univoco Unità Organizzativa iPA',
			'0202'=>'Indirizzo di Posta Elettronica Certificata',
			'0203'=>'eDelivery Network Participant identifier',
			'0204'=>'Leitweg-ID',
			'0205'=>'CODDEST',
			'0206'=>'Registre du Commerce et de l’Industrie : RCI',
			'0207'=>'PiLog Ontology Codification Identifier (POCI)',
			'0208'=>'Numero d\'entreprise / ondernemingsnummer / Unternehmensnummer',
			'0209'=>'GS1 identification keys',
			'0210'=>'CODICE FISCALE',
			'0211'=>'PARTITA IVA',
			'0212'=>'Finnish Organization Identifier',
			'0213'=>'Finnish Organization Value Add Tax Identifier',
			'0214'=>'Tradeplace TradePI Standard',
			'0215'=>'Net service ID',
			'0216'=>'OVTcode',
			'0217'=>'The Netherlands Chamber of Commerce and Industry establishment number',
			'0218'=>'Unified registration number (Latvia)',
			'0219'=>'Taxpayer registration code (Latvia)',
			'0220'=>'The Register of Natural Persons (Latvia)',
			'0221'=>'The registered number of the qualified invoice issuer',
			'0222'=>'Metadata Registry Support',
			'0223'=>'EU based company',
			'0224'=>'FTCTC CODE ROUTAGE',
			'0225'=>'FRCTC ELECTRONIC ADDRESS',
			'0226'=>'FRCTC Particulier',
			'0227'=>'NON - EU based company',
			'0228'=>'Répertoire des Entreprises et des Etablissements (RIDET)',
			'0229'=>'T.A.H.I.T.I (traitement automatique hiérarchisé des institutions de Tahiti et des îles)',
			'0230'=>'National e-Invoicing Framework',
			'0231'=>'Single taxable company (France)',
			'0232'=>'NOBB product number',
			'0233'=>'Description not known',
			'0234'=>'Toimitusosoite ID',
			'0235'=>'UAE Tax Identification Number (TIN)',
			'0236'=>'Description not known',
			'0237'=>'CPR (Danish person civil registration number)',
			'0238'=>'Plateforme.s agréée.s à la facturation électronique (PPF/PDP)'
		);

		if(!strlen($code))
			return $a;
		else if(isset($a[$code]))
			return $a[$code];
		else
			return '['.$code.' !]';
	}

	public function fxcUnitCode($code='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$code=trim((string)$code);

		$a=array(
			'10'=>'group',
			'11'=>'outfit',
			'13'=>'ration',
			'14'=>'shot',
			'15'=>'stick, military',
			'20'=>'twenty foot container',
			'21'=>'forty foot container',
			'22'=>'decilitre per gram',
			'23'=>'gram per cubic centimetre',
			'24'=>'theoretical pound',
			'25'=>'gram per square centimetre',
			'27'=>'theoretical ton',
			'28'=>'kilogram per square metre',
			'33'=>'kilopascal square metre per gram',
			'34'=>'kilopascal per millimetre',
			'35'=>'millilitre per square centimetre second',
			'37'=>'ounce per square foot',
			'38'=>'ounce per square foot per 0,01inch',
			'40'=>'millilitre per second',
			'41'=>'millilitre per minute',
			'56'=>'sitas',
			'57'=>'mesh',
			'58'=>'net kilogram',
			'59'=>'part per million',
			'60'=>'percent weight',
			'61'=>'part per billion (US)',
			'74'=>'millipascal',
			'77'=>'milli-inch',
			'80'=>'pound per square inch absolute',
			'81'=>'henry',
			'85'=>'foot pound-force',
			'87'=>'pound per cubic foot',
			'89'=>'poise',
			'91'=>'stokes',
			'1I'=>'fixed rate',
			'2A'=>'radian per second',
			'2B'=>'radian per second squared',
			'2C'=>'roentgen',
			'2G'=>'volt AC',
			'2H'=>'volt DC',
			'2I'=>'British thermal unit (international table) per hour',
			'2J'=>'cubic centimetre per second',
			'2K'=>'cubic foot per hour',
			'2L'=>'cubic foot per minute',
			'2M'=>'centimetre per second',
			'2N'=>'decibel',
			'2P'=>'kilobyte',
			'2Q'=>'kilobecquerel',
			'2R'=>'kilocurie',
			'2U'=>'megagram',
			'2X'=>'metre per minute',
			'2Y'=>'milliroentgen',
			'2Z'=>'millivolt',
			'3B'=>'megajoule',
			'3C'=>'manmonth',
			'4C'=>'centistokes',
			'4G'=>'microlitre',
			'4H'=>'micrometre (micron)',
			'4K'=>'milliampere',
			'4L'=>'megabyte',
			'4M'=>'milligram per hour',
			'4N'=>'megabecquerel',
			'4O'=>'microfarad',
			'4P'=>'newton per metre',
			'4Q'=>'ounce inch',
			'4R'=>'ounce foot',
			'4T'=>'picofarad',
			'4U'=>'pound per hour',
			'4W'=>'ton (US) per hour',
			'4X'=>'kilolitre per hour',
			'5A'=>'barrel (US) per minute',
			'5B'=>'batch',
			'5E'=>'MMSCF/day',
			'5J'=>'hydraulic horse power',
			'A10'=>'ampere square metre per joule second',
			'A11'=>'angstrom',
			'A12'=>'astronomical unit',
			'A13'=>'attojoule',
			'A14'=>'barn',
			'A15'=>'barn per electronvolt',
			'A16'=>'barn per steradian electronvolt',
			'A17'=>'barn per steradian',
			'A18'=>'becquerel per kilogram',
			'A19'=>'becquerel per cubic metre',
			'A2'=>'ampere per centimetre',
			'A20'=>'British thermal unit (international table) per second square foot degree Rankine',
			'A21'=>'British thermal unit (international table) per pound degree Rankine',
			'A22'=>'British thermal unit (international table) per second foot degree Rankine',
			'A23'=>'British thermal unit (international table) per hour square foot degree Rankine',
			'A24'=>'candela per square metre',
			'A26'=>'coulomb metre',
			'A27'=>'coulomb metre squared per volt',
			'A28'=>'coulomb per cubic centimetre',
			'A29'=>'coulomb per cubic metre',
			'A3'=>'ampere per millimetre',
			'A30'=>'coulomb per cubic millimetre',
			'A31'=>'coulomb per kilogram second',
			'A32'=>'coulomb per mole',
			'A33'=>'coulomb per square centimetre',
			'A34'=>'coulomb per square metre',
			'A35'=>'coulomb per square millimetre',
			'A36'=>'cubic centimetre per mole',
			'A37'=>'cubic decimetre per mole',
			'A38'=>'cubic metre per coulomb',
			'A39'=>'cubic metre per kilogram',
			'A4'=>'ampere per square centimetre',
			'A40'=>'cubic metre per mole',
			'A41'=>'ampere per square metre',
			'A42'=>'curie per kilogram',
			'A43'=>'deadweight tonnage',
			'A44'=>'decalitre',
			'A45'=>'decametre',
			'A47'=>'decitex',
			'A48'=>'degree Rankine',
			'A49'=>'denier',
			'A5'=>'ampere square metre',
			'A53'=>'electronvolt',
			'A54'=>'electronvolt per metre',
			'A55'=>'electronvolt square metre',
			'A56'=>'electronvolt square metre per kilogram',
			'A59'=>'8-part cloud cover',
			'A6'=>'ampere per square metre kelvin squared',
			'A68'=>'exajoule',
			'A69'=>'farad per metre',
			'A7'=>'ampere per square millimetre',
			'A70'=>'femtojoule',
			'A71'=>'femtometre',
			'A73'=>'foot per second squared',
			'A74'=>'foot pound-force per second',
			'A75'=>'freight ton',
			'A76'=>'gal',
			'A8'=>'ampere second',
			'A84'=>'gigacoulomb per cubic metre',
			'A85'=>'gigaelectronvolt',
			'A86'=>'gigahertz',
			'A87'=>'gigaohm',
			'A88'=>'gigaohm metre',
			'A89'=>'gigapascal',
			'A9'=>'rate',
			'A90'=>'gigawatt',
			'A91'=>'gon',
			'A93'=>'gram per cubic metre',
			'A94'=>'gram per mole',
			'A95'=>'gray',
			'A96'=>'gray per second',
			'A97'=>'hectopascal',
			'A98'=>'henry per metre',
			'A99'=>'bit',
			'AA'=>'ball',
			'AB'=>'bulk pack',
			'ACR'=>'acre',
			'ACT'=>'activity',
			'AD'=>'byte',
			'AE'=>'ampere per metre',
			'AH'=>'additional minute',
			'AI'=>'average minute per call',
			'AK'=>'fathom',
			'AL'=>'access line',
			'AMH'=>'ampere hour',
			'AMP'=>'ampere',
			'ANN'=>'year',
			'APZ'=>'troy ounce or apothecary ounce',
			'AQ'=>'anti-hemophilic factor (AHF) unit',
			'AS'=>'assortment',
			'ASM'=>'alcoholic strength by mass',
			'ASU'=>'alcoholic strength by volume',
			'ATM'=>'standard atmosphere',
			'AWG'=>'american wire gauge',
			'AY'=>'assembly',
			'AZ'=>'British thermal unit (international table) per pound',
			'B1'=>'barrel (US) per day',
			'B10'=>'bit per second',
			'B11'=>'joule per kilogram kelvin',
			'B12'=>'joule per metre',
			'B13'=>'joule per square metre',
			'B14'=>'joule per metre to the fourth power',
			'B15'=>'joule per mole',
			'B16'=>'joule per mole kelvin',
			'B17'=>'credit',
			'B18'=>'joule second',
			'B19'=>'digit',
			'B20'=>'joule square metre per kilogram',
			'B21'=>'kelvin per watt',
			'B22'=>'kiloampere',
			'B23'=>'kiloampere per square metre',
			'B24'=>'kiloampere per metre',
			'B25'=>'kilobecquerel per kilogram',
			'B26'=>'kilocoulomb',
			'B27'=>'kilocoulomb per cubic metre',
			'B28'=>'kilocoulomb per square metre',
			'B29'=>'kiloelectronvolt',
			'B3'=>'batting pound',
			'B30'=>'gibibit',
			'B31'=>'kilogram metre per second',
			'B32'=>'kilogram metre squared',
			'B33'=>'kilogram metre squared per second',
			'B34'=>'kilogram per cubic decimetre',
			'B35'=>'kilogram per litre',
			'B4'=>'barrel, imperial',
			'B41'=>'kilojoule per kelvin',
			'B42'=>'kilojoule per kilogram',
			'B43'=>'kilojoule per kilogram kelvin',
			'B44'=>'kilojoule per mole',
			'B45'=>'kilomole',
			'B46'=>'kilomole per cubic metre',
			'B47'=>'kilonewton',
			'B48'=>'kilonewton metre',
			'B49'=>'kiloohm',
			'B50'=>'kiloohm metre',
			'B52'=>'kilosecond',
			'B53'=>'kilosiemens',
			'B54'=>'kilosiemens per metre',
			'B55'=>'kilovolt per metre',
			'B56'=>'kiloweber per metre',
			'B57'=>'light year',
			'B58'=>'litre per mole',
			'B59'=>'lumen hour',
			'B60'=>'lumen per square metre',
			'B61'=>'lumen per watt',
			'B62'=>'lumen second',
			'B63'=>'lux hour',
			'B64'=>'lux second',
			'B66'=>'megaampere per square metre',
			'B67'=>'megabecquerel per kilogram',
			'B68'=>'gigabit',
			'B69'=>'megacoulomb per cubic metre',
			'B7'=>'cycle',
			'B70'=>'megacoulomb per square metre',
			'B71'=>'megaelectronvolt',
			'B72'=>'megagram per cubic metre',
			'B73'=>'meganewton',
			'B74'=>'meganewton metre',
			'B75'=>'megaohm',
			'B76'=>'megaohm metre',
			'B77'=>'megasiemens per metre',
			'B78'=>'megavolt',
			'B79'=>'megavolt per metre',
			'B8'=>'joule per cubic metre',
			'B80'=>'gigabit per second',
			'B81'=>'reciprocal metre squared reciprocal second',
			'B82'=>'inch per linear foot',
			'B83'=>'metre to the fourth power',
			'B84'=>'microampere',
			'B85'=>'microbar',
			'B86'=>'microcoulomb',
			'B87'=>'microcoulomb per cubic metre',
			'B88'=>'microcoulomb per square metre',
			'B89'=>'microfarad per metre',
			'B90'=>'microhenry',
			'B91'=>'microhenry per metre',
			'B92'=>'micronewton',
			'B93'=>'micronewton metre',
			'B94'=>'microohm',
			'B95'=>'microohm metre',
			'B96'=>'micropascal',
			'B97'=>'microradian',
			'B98'=>'microsecond',
			'B99'=>'microsiemens',
			'BAR'=>'bar [unit of pressure]',
			'BB'=>'base box',
			'BFT'=>'board foot',
			'BHP'=>'brake horse power',
			'BIL'=>'billion (EUR)',
			'BLD'=>'dry barrel (US)',
			'BLL'=>'barrel (US)',
			'BP'=>'hundred board foot',
			'BPM'=>'beats per minute',
			'BQL'=>'becquerel',
			'BTU'=>'British thermal unit (international table)',
			'BUA'=>'bushel (US)',
			'BUI'=>'bushel (UK)',
			'C0'=>'call',
			'C10'=>'millifarad',
			'C11'=>'milligal',
			'C12'=>'milligram per metre',
			'C13'=>'milligray',
			'C14'=>'millihenry',
			'C15'=>'millijoule',
			'C16'=>'millimetre per second',
			'C17'=>'millimetre squared per second',
			'C18'=>'millimole',
			'C19'=>'mole per kilogram',
			'C20'=>'millinewton',
			'C21'=>'kibibit',
			'C22'=>'millinewton per metre',
			'C23'=>'milliohm metre',
			'C24'=>'millipascal second',
			'C25'=>'milliradian',
			'C26'=>'millisecond',
			'C27'=>'millisiemens',
			'C28'=>'millisievert',
			'C29'=>'millitesla',
			'C3'=>'microvolt per metre',
			'C30'=>'millivolt per metre',
			'C31'=>'milliwatt',
			'C32'=>'milliwatt per square metre',
			'C33'=>'milliweber',
			'C34'=>'mole',
			'C35'=>'mole per cubic decimetre',
			'C36'=>'mole per cubic metre',
			'C37'=>'kilobit',
			'C38'=>'mole per litre',
			'C39'=>'nanoampere',
			'C40'=>'nanocoulomb',
			'C41'=>'nanofarad',
			'C42'=>'nanofarad per metre',
			'C43'=>'nanohenry',
			'C44'=>'nanohenry per metre',
			'C45'=>'nanometre',
			'C46'=>'nanoohm metre',
			'C47'=>'nanosecond',
			'C48'=>'nanotesla',
			'C49'=>'nanowatt',
			'C50'=>'neper',
			'C51'=>'neper per second',
			'C52'=>'picometre',
			'C53'=>'newton metre second',
			'C54'=>'newton metre squared per kilogram squared',
			'C55'=>'newton per square metre',
			'C56'=>'newton per square millimetre',
			'C57'=>'newton second',
			'C58'=>'newton second per metre',
			'C59'=>'octave',
			'C60'=>'ohm centimetre',
			'C61'=>'ohm metre',
			'C62'=>'one',
			'C63'=>'parsec',
			'C64'=>'pascal per kelvin',
			'C65'=>'pascal second',
			'C66'=>'pascal second per cubic metre',
			'C67'=>'pascal second per metre',
			'C68'=>'petajoule',
			'C69'=>'phon',
			'C7'=>'centipoise',
			'C70'=>'picoampere',
			'C71'=>'picocoulomb',
			'C72'=>'picofarad per metre',
			'C73'=>'picohenry',
			'C74'=>'kilobit per second',
			'C75'=>'picowatt',
			'C76'=>'picowatt per square metre',
			'C78'=>'pound-force',
			'C79'=>'kilovolt ampere hour',
			'C8'=>'millicoulomb per kilogram',
			'C80'=>'rad',
			'C81'=>'radian',
			'C82'=>'radian square metre per mole',
			'C83'=>'radian square metre per kilogram',
			'C84'=>'radian per metre',
			'C85'=>'reciprocal angstrom',
			'C86'=>'reciprocal cubic metre',
			'C87'=>'reciprocal cubic metre per second',
			'C88'=>'reciprocal electron volt per cubic metre',
			'C89'=>'reciprocal henry',
			'C9'=>'coil group',
			'C90'=>'reciprocal joule per cubic metre',
			'C91'=>'reciprocal kelvin or kelvin to the power minus one',
			'C92'=>'reciprocal metre',
			'C93'=>'reciprocal square metre',
			'C94'=>'reciprocal minute',
			'C95'=>'reciprocal mole',
			'C96'=>'reciprocal pascal or pascal to the power minus one',
			'C97'=>'reciprocal second',
			'C99'=>'reciprocal second per metre squared',
			'CCT'=>'carrying capacity in metric ton',
			'CDL'=>'candela',
			'CEL'=>'degree Celsius',
			'CEN'=>'hundred',
			'CG'=>'card',
			'CGM'=>'centigram',
			'CKG'=>'coulomb per kilogram',
			'CLF'=>'hundred leave',
			'CLT'=>'centilitre',
			'CMK'=>'square centimetre',
			'CMQ'=>'cubic centimetre',
			'CMT'=>'centimetre',
			'CNP'=>'hundred pack',
			'CNT'=>'cental (UK)',
			'COU'=>'coulomb',
			'CTG'=>'content gram',
			'CTM'=>'metric carat',
			'CTN'=>'content ton (metric)',
			'CUR'=>'curie',
			'CWA'=>'hundred pound (cwt) / hundred weight (US)',
			'CWI'=>'hundred weight (UK)',
			'D03'=>'kilowatt hour per hour',
			'D04'=>'lot  [unit of weight]',
			'D1'=>'reciprocal second per steradian',
			'D10'=>'siemens per metre',
			'D11'=>'mebibit',
			'D12'=>'siemens square metre per mole',
			'D13'=>'sievert',
			'D15'=>'sone',
			'D16'=>'square centimetre per erg',
			'D17'=>'square centimetre per steradian erg',
			'D18'=>'metre kelvin',
			'D19'=>'square metre kelvin per watt',
			'D2'=>'reciprocal second per steradian metre squared',
			'D20'=>'square metre per joule',
			'D21'=>'square metre per kilogram',
			'D22'=>'square metre per mole',
			'D23'=>'pen gram (protein)',
			'D24'=>'square metre per steradian',
			'D25'=>'square metre per steradian joule',
			'D26'=>'square metre per volt second',
			'D27'=>'steradian',
			'D29'=>'terahertz',
			'D30'=>'terajoule',
			'D31'=>'terawatt',
			'D32'=>'terawatt hour',
			'D33'=>'tesla',
			'D34'=>'tex',
			'D36'=>'megabit',
			'D41'=>'tonne per cubic metre',
			'D42'=>'tropical year',
			'D43'=>'unified atomic mass unit',
			'D44'=>'var',
			'D45'=>'volt squared per kelvin squared',
			'D46'=>'volt - ampere',
			'D47'=>'volt per centimetre',
			'D48'=>'volt per kelvin',
			'D49'=>'millivolt per kelvin',
			'D5'=>'kilogram per square centimetre',
			'D50'=>'volt per metre',
			'D51'=>'volt per millimetre',
			'D52'=>'watt per kelvin',
			'D53'=>'watt per metre kelvin',
			'D54'=>'watt per square metre',
			'D55'=>'watt per square metre kelvin',
			'D56'=>'watt per square metre kelvin to the fourth power',
			'D57'=>'watt per steradian',
			'D58'=>'watt per steradian square metre',
			'D59'=>'weber per metre',
			'D6'=>'roentgen per second',
			'D60'=>'weber per millimetre',
			'D61'=>'minute [unit of angle]',
			'D62'=>'second [unit of angle]',
			'D63'=>'book',
			'D65'=>'round',
			'D68'=>'number of words',
			'D69'=>'inch to the fourth power',
			'D73'=>'joule square metre',
			'D74'=>'kilogram per mole',
			'D77'=>'megacoulomb',
			'D78'=>'megajoule per second',
			'D80'=>'microwatt',
			'D81'=>'microtesla',
			'D82'=>'microvolt',
			'D83'=>'millinewton metre',
			'D85'=>'microwatt per square metre',
			'D86'=>'millicoulomb',
			'D87'=>'millimole per kilogram',
			'D88'=>'millicoulomb per cubic metre',
			'D89'=>'millicoulomb per square metre',
			'D91'=>'rem',
			'D93'=>'second per cubic metre',
			'D94'=>'second per cubic metre radian',
			'D95'=>'joule per gram',
			'DAA'=>'decare',
			'DAD'=>'ten day',
			'DAY'=>'day',
			'DB'=>'dry pound',
			'DBM'=>'Decibel-milliwatts',
			'DBW'=>'Decibel watt',
			'DD'=>'degree [unit of angle]',
			'DEC'=>'decade',
			'DG'=>'decigram',
			'DJ'=>'decagram',
			'DLT'=>'decilitre',
			'DMA'=>'cubic decametre',
			'DMK'=>'square decimetre',
			'DMO'=>'standard kilolitre',
			'DMQ'=>'cubic decimetre',
			'DMT'=>'decimetre',
			'DN'=>'decinewton metre',
			'DPC'=>'dozen piece',
			'DPR'=>'dozen pair',
			'DPT'=>'displacement tonnage',
			'DRA'=>'dram (US)',
			'DRI'=>'dram (UK)',
			'DRL'=>'dozen roll',
			'DT'=>'dry ton',
			'DTN'=>'decitonne',
			'DWT'=>'pennyweight',
			'DZN'=>'dozen',
			'DZP'=>'dozen pack',
			'E01'=>'newton per square centimetre',
			'E07'=>'megawatt hour per hour',
			'E08'=>'megawatt per hertz',
			'E09'=>'milliampere hour',
			'E10'=>'degree day',
			'E12'=>'mille',
			'E14'=>'kilocalorie (international table)',
			'E15'=>'kilocalorie (thermochemical) per hour',
			'E16'=>'million Btu(IT) per hour',
			'E17'=>'cubic foot per second',
			'E18'=>'tonne per hour',
			'E19'=>'ping',
			'E20'=>'megabit per second',
			'E21'=>'shares',
			'E22'=>'TEU',
			'E23'=>'tyre',
			'E25'=>'active unit',
			'E27'=>'dose',
			'E28'=>'air dry ton',
			'E30'=>'strand',
			'E31'=>'square metre per litre',
			'E32'=>'litre per hour',
			'E33'=>'foot per thousand',
			'E34'=>'gigabyte',
			'E35'=>'terabyte',
			'E36'=>'petabyte',
			'E37'=>'pixel',
			'E38'=>'megapixel',
			'E39'=>'dots per inch',
			'E4'=>'gross kilogram',
			'E40'=>'part per hundred thousand',
			'E41'=>'kilogram-force per square millimetre',
			'E42'=>'kilogram-force per square centimetre',
			'E43'=>'joule per square centimetre',
			'E44'=>'kilogram-force metre per square centimetre',
			'E45'=>'milliohm',
			'E46'=>'kilowatt hour per cubic metre',
			'E47'=>'kilowatt hour per kelvin',
			'E48'=>'service unit',
			'E49'=>'working day',
			'E50'=>'accounting unit',
			'E51'=>'job',
			'E52'=>'run foot',
			'E53'=>'test',
			'E54'=>'trip',
			'E55'=>'use',
			'E56'=>'well',
			'E57'=>'zone',
			'E58'=>'exabit per second',
			'E59'=>'exbibyte',
			'E60'=>'pebibyte',
			'E61'=>'tebibyte',
			'E62'=>'gibibyte',
			'E63'=>'mebibyte',
			'E64'=>'kibibyte',
			'E65'=>'exbibit per metre',
			'E66'=>'exbibit per square metre',
			'E67'=>'exbibit per cubic metre',
			'E68'=>'gigabyte per second',
			'E69'=>'gibibit per metre',
			'E70'=>'gibibit per square metre',
			'E71'=>'gibibit per cubic metre',
			'E72'=>'kibibit per metre',
			'E73'=>'kibibit per square metre',
			'E74'=>'kibibit per cubic metre',
			'E75'=>'mebibit per metre',
			'E76'=>'mebibit per square metre',
			'E77'=>'mebibit per cubic metre',
			'E78'=>'petabit',
			'E79'=>'petabit per second',
			'E80'=>'pebibit per metre',
			'E81'=>'pebibit per square metre',
			'E82'=>'pebibit per cubic metre',
			'E83'=>'terabit',
			'E84'=>'terabit per second',
			'E85'=>'tebibit per metre',
			'E86'=>'tebibit per cubic metre',
			'E87'=>'tebibit per square metre',
			'E88'=>'bit per metre',
			'E89'=>'bit per square metre',
			'E90'=>'reciprocal centimetre',
			'E91'=>'reciprocal day',
			'E92'=>'cubic decimetre per hour',
			'E93'=>'kilogram per hour',
			'E94'=>'kilomole per second',
			'E95'=>'mole per second',
			'E96'=>'degree per second',
			'E97'=>'millimetre per degree Celcius metre',
			'E98'=>'degree Celsius per kelvin',
			'E99'=>'hectopascal per bar',
			'EA'=>'each',
			'EB'=>'electronic mail box',
			'EQ'=>'equivalent gallon',
			'F01'=>'bit per cubic metre',
			'F02'=>'kelvin per kelvin',
			'F03'=>'kilopascal per bar',
			'F04'=>'millibar per bar',
			'F05'=>'megapascal per bar',
			'F06'=>'poise per bar',
			'F07'=>'pascal per bar',
			'F08'=>'milliampere per inch',
			'F10'=>'kelvin per hour',
			'F11'=>'kelvin per minute',
			'F12'=>'kelvin per second',
			'F13'=>'slug',
			'F14'=>'gram per kelvin',
			'F15'=>'kilogram per kelvin',
			'F16'=>'milligram per kelvin',
			'F17'=>'pound-force per foot',
			'F18'=>'kilogram square centimetre',
			'F19'=>'kilogram square millimetre',
			'F20'=>'pound inch squared',
			'F21'=>'pound-force inch',
			'F22'=>'pound-force foot per ampere',
			'F23'=>'gram per cubic decimetre',
			'F24'=>'kilogram per kilomol',
			'F25'=>'gram per hertz',
			'F26'=>'gram per day',
			'F27'=>'gram per hour',
			'F28'=>'gram per minute',
			'F29'=>'gram per second',
			'F30'=>'kilogram per day',
			'F31'=>'kilogram per minute',
			'F32'=>'milligram per day',
			'F33'=>'milligram per minute',
			'F34'=>'milligram per second',
			'F35'=>'gram per day kelvin',
			'F36'=>'gram per hour kelvin',
			'F37'=>'gram per minute kelvin',
			'F38'=>'gram per second kelvin',
			'F39'=>'kilogram per day kelvin',
			'F40'=>'kilogram per hour kelvin',
			'F41'=>'kilogram per minute kelvin',
			'F42'=>'kilogram per second kelvin',
			'F43'=>'milligram per day kelvin',
			'F44'=>'milligram per hour kelvin',
			'F45'=>'milligram per minute kelvin',
			'F46'=>'milligram per second kelvin',
			'F47'=>'newton per millimetre',
			'F48'=>'pound-force per inch',
			'F49'=>'rod [unit of distance]',
			'F50'=>'micrometre per kelvin',
			'F51'=>'centimetre per kelvin',
			'F52'=>'metre per kelvin',
			'F53'=>'millimetre per kelvin',
			'F54'=>'milliohm per metre',
			'F55'=>'ohm per mile (statute mile)',
			'F56'=>'ohm per kilometre',
			'F57'=>'milliampere per pound-force per square inch',
			'F58'=>'reciprocal bar',
			'F59'=>'milliampere per bar',
			'F60'=>'degree Celsius per bar',
			'F61'=>'kelvin per bar',
			'F62'=>'gram per day bar',
			'F63'=>'gram per hour bar',
			'F64'=>'gram per minute bar',
			'F65'=>'gram per second bar',
			'F66'=>'kilogram per day bar',
			'F67'=>'kilogram per hour bar',
			'F68'=>'kilogram per minute bar',
			'F69'=>'kilogram per second bar',
			'F70'=>'milligram per day bar',
			'F71'=>'milligram per hour bar',
			'F72'=>'milligram per minute bar',
			'F73'=>'milligram per second bar',
			'F74'=>'gram per bar',
			'F75'=>'milligram per bar',
			'F76'=>'milliampere per millimetre',
			'F77'=>'pascal second per kelvin',
			'F78'=>'inch of water',
			'F79'=>'inch of mercury',
			'F80'=>'water horse power',
			'F81'=>'bar per kelvin',
			'F82'=>'hectopascal per kelvin',
			'F83'=>'kilopascal per kelvin',
			'F84'=>'millibar per kelvin',
			'F85'=>'megapascal per kelvin',
			'F86'=>'poise per kelvin',
			'F87'=>'volt per litre minute',
			'F88'=>'newton centimetre',
			'F89'=>'newton metre per degree',
			'F90'=>'newton metre per ampere',
			'F91'=>'bar litre per second',
			'F92'=>'bar cubic metre per second',
			'F93'=>'hectopascal litre per second',
			'F94'=>'hectopascal cubic metre per second',
			'F95'=>'millibar litre per second',
			'F96'=>'millibar cubic metre per second',
			'F97'=>'megapascal litre per second',
			'F98'=>'megapascal cubic metre per second',
			'F99'=>'pascal litre per second',
			'FAH'=>'degree Fahrenheit',
			'FAR'=>'farad',
			'FBM'=>'fibre metre',
			'FC'=>'thousand cubic foot',
			'FF'=>'hundred cubic metre',
			'FH'=>'micromole',
			'FIT'=>'failures in time',
			'FL'=>'flake ton',
			'FNU'=>'Formazin nephelometric unit',
			'FOT'=>'foot',
			'FP'=>'pound per square foot',
			'FR'=>'foot per minute',
			'FS'=>'foot per second',
			'FTK'=>'square foot',
			'FTQ'=>'cubic foot',
			'G01'=>'pascal cubic metre per second',
			'G04'=>'centimetre per bar',
			'G05'=>'metre per bar',
			'G06'=>'millimetre per bar',
			'G08'=>'square inch per second',
			'G09'=>'square metre per second kelvin',
			'G10'=>'stokes per kelvin',
			'G11'=>'gram per cubic centimetre bar',
			'G12'=>'gram per cubic decimetre bar',
			'G13'=>'gram per litre bar',
			'G14'=>'gram per cubic metre bar',
			'G15'=>'gram per millilitre bar',
			'G16'=>'kilogram per cubic centimetre bar',
			'G17'=>'kilogram per litre bar',
			'G18'=>'kilogram per cubic metre bar',
			'G19'=>'newton metre per kilogram',
			'G2'=>'US gallon per minute',
			'G20'=>'pound-force foot per pound',
			'G21'=>'cup [unit of volume]',
			'G23'=>'peck',
			'G24'=>'tablespoon (US)',
			'G25'=>'teaspoon (US)',
			'G26'=>'stere',
			'G27'=>'cubic centimetre per kelvin',
			'G28'=>'litre per kelvin',
			'G29'=>'cubic metre per kelvin',
			'G3'=>'Imperial gallon per minute',
			'G30'=>'millilitre per kelvin',
			'G31'=>'kilogram per cubic centimetre',
			'G32'=>'ounce (avoirdupois) per cubic yard',
			'G33'=>'gram per cubic centimetre kelvin',
			'G34'=>'gram per cubic decimetre kelvin',
			'G35'=>'gram per litre kelvin',
			'G36'=>'gram per cubic metre kelvin',
			'G37'=>'gram per millilitre kelvin',
			'G38'=>'kilogram per cubic centimetre kelvin',
			'G39'=>'kilogram per litre kelvin',
			'G40'=>'kilogram per cubic metre kelvin',
			'G41'=>'square metre per second bar',
			'G42'=>'microsiemens per centimetre',
			'G43'=>'microsiemens per metre',
			'G44'=>'nanosiemens per centimetre',
			'G45'=>'nanosiemens per metre',
			'G46'=>'stokes per bar',
			'G47'=>'cubic centimetre per day',
			'G48'=>'cubic centimetre per hour',
			'G49'=>'cubic centimetre per minute',
			'G50'=>'gallon (US) per hour',
			'G51'=>'litre per second',
			'G52'=>'cubic metre per day',
			'G53'=>'cubic metre per minute',
			'G54'=>'millilitre per day',
			'G55'=>'millilitre per hour',
			'G56'=>'cubic inch per hour',
			'G57'=>'cubic inch per minute',
			'G58'=>'cubic inch per second',
			'G59'=>'milliampere per litre minute',
			'G60'=>'volt per bar',
			'G61'=>'cubic centimetre per day kelvin',
			'G62'=>'cubic centimetre per hour kelvin',
			'G63'=>'cubic centimetre per minute kelvin',
			'G64'=>'cubic centimetre per second kelvin',
			'G65'=>'litre per day kelvin',
			'G66'=>'litre per hour kelvin',
			'G67'=>'litre per minute kelvin',
			'G68'=>'litre per second kelvin',
			'G69'=>'cubic metre per day kelvin',
			'G70'=>'cubic metre per hour kelvin',
			'G71'=>'cubic metre per minute kelvin',
			'G72'=>'cubic metre per second kelvin',
			'G73'=>'millilitre per day kelvin',
			'G74'=>'millilitre per hour kelvin',
			'G75'=>'millilitre per minute kelvin',
			'G76'=>'millilitre per second kelvin',
			'G77'=>'millimetre to the fourth power',
			'G78'=>'cubic centimetre per day bar',
			'G79'=>'cubic centimetre per hour bar',
			'G80'=>'cubic centimetre per minute bar',
			'G81'=>'cubic centimetre per second bar',
			'G82'=>'litre per day bar',
			'G83'=>'litre per hour bar',
			'G84'=>'litre per minute bar',
			'G85'=>'litre per second bar',
			'G86'=>'cubic metre per day bar',
			'G87'=>'cubic metre per hour bar',
			'G88'=>'cubic metre per minute bar',
			'G89'=>'cubic metre per second bar',
			'G90'=>'millilitre per day bar',
			'G91'=>'millilitre per hour bar',
			'G92'=>'millilitre per minute bar',
			'G93'=>'millilitre per second bar',
			'G94'=>'cubic centimetre per bar',
			'G95'=>'litre per bar',
			'G96'=>'cubic metre per bar',
			'G97'=>'millilitre per bar',
			'G98'=>'microhenry per kiloohm',
			'G99'=>'microhenry per ohm',
			'GB'=>'gallon (US) per day',
			'GBQ'=>'gigabecquerel',
			'GDW'=>'gram, dry weight',
			'GE'=>'pound per gallon (US)',
			'GF'=>'gram per metre (gram per 100 centimetres)',
			'GFI'=>'gram of fissile isotope',
			'GGR'=>'great gross',
			'GIA'=>'gill (US)',
			'GIC'=>'gram, including container',
			'GII'=>'gill (UK)',
			'GIP'=>'gram, including inner packaging',
			'GJ'=>'gram per millilitre',
			'GL'=>'gram per litre',
			'GLD'=>'dry gallon (US)',
			'GLI'=>'gallon (UK)',
			'GLL'=>'gallon (US)',
			'GM'=>'gram per square metre',
			'GO'=>'milligram per square metre',
			'GP'=>'milligram per cubic metre',
			'GQ'=>'microgram per cubic metre',
			'GRM'=>'gram',
			'GRN'=>'grain',
			'GRO'=>'gross',
			'GV'=>'gigajoule',
			'GWH'=>'gigawatt hour',
			'H03'=>'henry per kiloohm',
			'H04'=>'henry per ohm',
			'H05'=>'millihenry per kiloohm',
			'H06'=>'millihenry per ohm',
			'H07'=>'pascal second per bar',
			'H08'=>'microbecquerel',
			'H09'=>'reciprocal year',
			'H10'=>'reciprocal hour',
			'H11'=>'reciprocal month',
			'H12'=>'degree Celsius per hour',
			'H13'=>'degree Celsius per minute',
			'H14'=>'degree Celsius per second',
			'H15'=>'square centimetre per gram',
			'H16'=>'square decametre',
			'H18'=>'square hectometre',
			'H19'=>'cubic hectometre',
			'H20'=>'cubic kilometre',
			'H21'=>'blank',
			'H22'=>'volt square inch per pound-force',
			'H23'=>'volt per inch',
			'H24'=>'volt per microsecond',
			'H25'=>'percent per kelvin',
			'H26'=>'ohm per metre',
			'H27'=>'degree per metre',
			'H28'=>'microfarad per kilometre',
			'H29'=>'microgram per litre',
			'H30'=>'square micrometre (square micron)',
			'H31'=>'ampere per kilogram',
			'H32'=>'ampere squared second',
			'H33'=>'farad per kilometre',
			'H34'=>'hertz metre',
			'H35'=>'kelvin metre per watt',
			'H36'=>'megaohm per kilometre',
			'H37'=>'megaohm per metre',
			'H38'=>'megaampere',
			'H39'=>'megahertz kilometre',
			'H40'=>'newton per ampere',
			'H41'=>'newton metre watt to the power minus 0,5',
			'H42'=>'pascal per metre',
			'H43'=>'siemens per centimetre',
			'H44'=>'teraohm',
			'H45'=>'volt second per metre',
			'H46'=>'volt per second',
			'H47'=>'watt per cubic metre',
			'H48'=>'attofarad',
			'H49'=>'centimetre per hour',
			'H50'=>'reciprocal cubic centimetre',
			'H51'=>'decibel per kilometre',
			'H52'=>'decibel per metre',
			'H53'=>'kilogram per bar',
			'H54'=>'kilogram per cubic decimetre kelvin',
			'H55'=>'kilogram per cubic decimetre bar',
			'H56'=>'kilogram per square metre second',
			'H57'=>'inch per two pi radiant',
			'H58'=>'metre per volt second',
			'H59'=>'square metre per newton',
			'H60'=>'cubic metre per cubic metre',
			'H61'=>'millisiemens per centimetre',
			'H62'=>'millivolt per minute',
			'H63'=>'milligram per square centimetre',
			'H64'=>'milligram per gram',
			'H65'=>'millilitre per cubic metre',
			'H66'=>'millimetre per year',
			'H67'=>'millimetre per hour',
			'H68'=>'millimole per gram',
			'H69'=>'picopascal per kilometre',
			'H70'=>'picosecond',
			'H71'=>'percent per month',
			'H72'=>'percent per hectobar',
			'H73'=>'percent per decakelvin',
			'H74'=>'watt per metre',
			'H75'=>'decapascal',
			'H76'=>'gram per millimetre',
			'H77'=>'module width',
			'H79'=>'French gauge',
			'H80'=>'rack unit',
			'H81'=>'millimetre per minute',
			'H82'=>'big point',
			'H83'=>'litre per kilogram',
			'H84'=>'gram millimetre',
			'H85'=>'reciprocal week',
			'H87'=>'piece',
			'H88'=>'megaohm kilometre',
			'H89'=>'percent per ohm',
			'H90'=>'percent per degree',
			'H91'=>'percent per ten thousand',
			'H92'=>'percent per one hundred thousand',
			'H93'=>'percent per hundred',
			'H94'=>'percent per thousand',
			'H95'=>'percent per volt',
			'H96'=>'percent per bar',
			'H98'=>'percent per inch',
			'H99'=>'percent per metre',
			'HA'=>'hank',
			'HAD'=>'Piece Day',
			'HBA'=>'hectobar',
			'HBX'=>'hundred boxes',
			'HC'=>'hundred count',
			'HDW'=>'hundred kilogram, dry weight',
			'HEA'=>'head',
			'HGM'=>'hectogram',
			'HH'=>'hundred cubic foot',
			'HIU'=>'hundred international unit',
			'HKM'=>'hundred kilogram, net mass',
			'HLT'=>'hectolitre',
			'HM'=>'mile per hour (statute mile)',
			'HMO'=>'Piece Month',
			'HMQ'=>'million cubic metre',
			'HMT'=>'hectometre',
			'HPA'=>'hectolitre of pure alcohol',
			'HTZ'=>'hertz',
			'HUR'=>'hour',
			'HWE'=>'Piece Week',
			'IA'=>'inch pound (pound inch)',
			'IE'=>'person',
			'INH'=>'inch',
			'INK'=>'square inch',
			'INQ'=>'cubic inch',
			'ISD'=>'international sugar degree',
			'IU'=>'inch per second',
			'IUG'=>'international unit per gram',
			'IV'=>'inch per second squared',
			'J10'=>'percent per millimetre',
			'J12'=>'per mille per psi',
			'J13'=>'degree API',
			'J14'=>'degree Baume (origin scale)',
			'J15'=>'degree Baume (US heavy)',
			'J16'=>'degree Baume (US light)',
			'J17'=>'degree Balling',
			'J18'=>'degree Brix',
			'J19'=>'degree Fahrenheit hour square foot per British thermal unit (thermochemical)',
			'J2'=>'joule per kilogram',
			'J20'=>'degree Fahrenheit per kelvin',
			'J21'=>'degree Fahrenheit per bar',
			'J22'=>'degree Fahrenheit hour square foot per British thermal unit (international table)',
			'J23'=>'degree Fahrenheit per hour',
			'J24'=>'degree Fahrenheit per minute',
			'J25'=>'degree Fahrenheit per second',
			'J26'=>'reciprocal degree Fahrenheit',
			'J27'=>'degree Oechsle',
			'J28'=>'degree Rankine per hour',
			'J29'=>'degree Rankine per minute',
			'J30'=>'degree Rankine per second',
			'J31'=>'degree Twaddell',
			'J32'=>'micropoise',
			'J33'=>'microgram per kilogram',
			'J34'=>'microgram per cubic metre kelvin',
			'J35'=>'microgram per cubic metre bar',
			'J36'=>'microlitre per litre',
			'J38'=>'baud',
			'J39'=>'British thermal unit (mean)',
			'J40'=>'British thermal unit (international table) foot per hour square foot degree Fahrenheit',
			'J41'=>'British thermal unit (international table) inch per hour square foot degree Fahrenheit',
			'J42'=>'British thermal unit (international table) inch per second square foot degree Fahrenheit',
			'J43'=>'British thermal unit (international table) per pound degree Fahrenheit',
			'J44'=>'British thermal unit (international table) per minute',
			'J45'=>'British thermal unit (international table) per second',
			'J46'=>'British thermal unit (thermochemical) foot per hour square foot degree Fahrenheit',
			'J47'=>'British thermal unit (thermochemical) per hour',
			'J48'=>'British thermal unit (thermochemical) inch per hour square foot degree Fahrenheit',
			'J49'=>'British thermal unit (thermochemical) inch per second square foot degree Fahrenheit',
			'J50'=>'British thermal unit (thermochemical) per pound degree Fahrenheit',
			'J51'=>'British thermal unit (thermochemical) per minute',
			'J52'=>'British thermal unit (thermochemical) per second',
			'J53'=>'coulomb square metre per kilogram',
			'J54'=>'megabaud',
			'J55'=>'watt second',
			'J56'=>'bar per bar',
			'J57'=>'barrel (UK petroleum)',
			'J58'=>'barrel (UK petroleum) per minute',
			'J59'=>'barrel (UK petroleum) per day',
			'J60'=>'barrel (UK petroleum) per hour',
			'J61'=>'barrel (UK petroleum) per second',
			'J62'=>'barrel (US petroleum) per hour',
			'J63'=>'barrel (US petroleum) per second',
			'J64'=>'bushel (UK) per day',
			'J65'=>'bushel (UK) per hour',
			'J66'=>'bushel (UK) per minute',
			'J67'=>'bushel (UK) per second',
			'J68'=>'bushel (US dry) per day',
			'J69'=>'bushel (US dry) per hour',
			'J70'=>'bushel (US dry) per minute',
			'J71'=>'bushel (US dry) per second',
			'J72'=>'centinewton metre',
			'J73'=>'centipoise per kelvin',
			'J74'=>'centipoise per bar',
			'J75'=>'calorie (mean)',
			'J76'=>'calorie (international table) per gram degree Celsius',
			'J78'=>'calorie (thermochemical) per centimetre second degree Celsius',
			'J79'=>'calorie (thermochemical) per gram degree Celsius',
			'J81'=>'calorie (thermochemical) per minute',
			'J82'=>'calorie (thermochemical) per second',
			'J83'=>'clo',
			'J84'=>'centimetre per second kelvin',
			'J85'=>'centimetre per second bar',
			'J87'=>'cubic centimetre per cubic metre',
			'J90'=>'cubic decimetre per day',
			'J91'=>'cubic decimetre per cubic metre',
			'J92'=>'cubic decimetre per minute',
			'J93'=>'cubic decimetre per second',
			'J95'=>'ounce (UK fluid) per day',
			'J96'=>'ounce (UK fluid) per hour',
			'J97'=>'ounce (UK fluid) per minute',
			'J98'=>'ounce (UK fluid) per second',
			'J99'=>'ounce (US fluid) per day',
			'JE'=>'joule per kelvin',
			'JK'=>'megajoule per kilogram',
			'JM'=>'megajoule per cubic metre',
			'JNT'=>'pipeline joint',
			'JOU'=>'joule',
			'JPS'=>'hundred metre',
			'JWL'=>'number of jewels',
			'K1'=>'kilowatt demand',
			'K10'=>'ounce (US fluid) per hour',
			'K11'=>'ounce (US fluid) per minute',
			'K12'=>'ounce (US fluid) per second',
			'K13'=>'foot per degree Fahrenheit',
			'K14'=>'foot per hour',
			'K15'=>'foot pound-force per hour',
			'K16'=>'foot pound-force per minute',
			'K17'=>'foot per psi',
			'K18'=>'foot per second degree Fahrenheit',
			'K19'=>'foot per second psi',
			'K2'=>'kilovolt ampere reactive demand',
			'K20'=>'reciprocal cubic foot',
			'K21'=>'cubic foot per degree Fahrenheit',
			'K22'=>'cubic foot per day',
			'K23'=>'cubic foot per psi',
			'K26'=>'gallon (UK) per day',
			'K27'=>'gallon (UK) per hour',
			'K28'=>'gallon (UK) per second',
			'K3'=>'kilovolt ampere reactive hour',
			'K30'=>'gallon (US liquid) per second',
			'K31'=>'gram-force per square centimetre',
			'K32'=>'gill (UK) per day',
			'K33'=>'gill (UK) per hour',
			'K34'=>'gill (UK) per minute',
			'K35'=>'gill (UK) per second',
			'K36'=>'gill (US) per day',
			'K37'=>'gill (US) per hour',
			'K38'=>'gill (US) per minute',
			'K39'=>'gill (US) per second',
			'K40'=>'standard acceleration of free fall',
			'K41'=>'grain per gallon (US)',
			'K42'=>'horsepower (boiler)',
			'K43'=>'horsepower (electric)',
			'K45'=>'inch per degree Fahrenheit',
			'K46'=>'inch per psi',
			'K47'=>'inch per second degree Fahrenheit',
			'K48'=>'inch per second psi',
			'K49'=>'reciprocal cubic inch',
			'K50'=>'kilobaud',
			'K51'=>'kilocalorie (mean)',
			'K52'=>'kilocalorie (international table) per hour metre degree Celsius',
			'K53'=>'kilocalorie (thermochemical)',
			'K54'=>'kilocalorie (thermochemical) per minute',
			'K55'=>'kilocalorie (thermochemical) per second',
			'K58'=>'kilomole per hour',
			'K59'=>'kilomole per cubic metre kelvin',
			'K6'=>'kilolitre',
			'K60'=>'kilomole per cubic metre bar',
			'K61'=>'kilomole per minute',
			'K62'=>'litre per litre',
			'K63'=>'reciprocal litre',
			'K64'=>'pound (avoirdupois) per degree Fahrenheit',
			'K65'=>'pound (avoirdupois) square foot',
			'K66'=>'pound (avoirdupois) per day',
			'K67'=>'pound per foot hour',
			'K68'=>'pound per foot second',
			'K69'=>'pound (avoirdupois) per cubic foot degree Fahrenheit',
			'K70'=>'pound (avoirdupois) per cubic foot psi',
			'K71'=>'pound (avoirdupois) per gallon (UK)',
			'K73'=>'pound (avoirdupois) per hour degree Fahrenheit',
			'K74'=>'pound (avoirdupois) per hour psi',
			'K75'=>'pound (avoirdupois) per cubic inch degree Fahrenheit',
			'K76'=>'pound (avoirdupois) per cubic inch psi',
			'K77'=>'pound (avoirdupois) per psi',
			'K78'=>'pound (avoirdupois) per minute',
			'K79'=>'pound (avoirdupois) per minute degree Fahrenheit',
			'K80'=>'pound (avoirdupois) per minute psi',
			'K81'=>'pound (avoirdupois) per second',
			'K82'=>'pound (avoirdupois) per second degree Fahrenheit',
			'K83'=>'pound (avoirdupois) per second psi',
			'K84'=>'pound per cubic yard',
			'K85'=>'pound-force per square foot',
			'K86'=>'pound-force per square inch degree Fahrenheit',
			'K87'=>'psi cubic inch per second',
			'K88'=>'psi litre per second',
			'K89'=>'psi cubic metre per second',
			'K90'=>'psi cubic yard per second',
			'K91'=>'pound-force second per square foot',
			'K92'=>'pound-force second per square inch',
			'K93'=>'reciprocal psi',
			'K94'=>'quart (UK liquid) per day',
			'K95'=>'quart (UK liquid) per hour',
			'K96'=>'quart (UK liquid) per minute',
			'K97'=>'quart (UK liquid) per second',
			'K98'=>'quart (US liquid) per day',
			'K99'=>'quart (US liquid) per hour',
			'KA'=>'cake',
			'KAT'=>'katal',
			'KB'=>'kilocharacter',
			'KBA'=>'kilobar',
			'KCC'=>'kilogram of choline chloride',
			'KDW'=>'kilogram drained net weight',
			'KEL'=>'kelvin',
			'KGM'=>'kilogram',
			'KGS'=>'kilogram per second',
			'KHY'=>'kilogram of hydrogen peroxide',
			'KHZ'=>'kilohertz',
			'KI'=>'kilogram per millimetre width',
			'KIC'=>'kilogram, including container',
			'KIP'=>'kilogram, including inner packaging',
			'KJ'=>'kilosegment',
			'KJO'=>'kilojoule',
			'KL'=>'kilogram per metre',
			'KLK'=>'lactic dry material percentage',
			'KLX'=>'kilolux',
			'KMA'=>'kilogram of methylamine',
			'KMH'=>'kilometre per hour',
			'KMK'=>'square kilometre',
			'KMQ'=>'kilogram per cubic metre',
			'KMT'=>'kilometre',
			'KNI'=>'kilogram of nitrogen',
			'KNM'=>'kilonewton per square metre',
			'KNS'=>'kilogram named substance',
			'KNT'=>'knot',
			'KO'=>'milliequivalence caustic potash per gram of product',
			'KPA'=>'kilopascal',
			'KPH'=>'kilogram of potassium hydroxide (caustic potash)',
			'KPO'=>'kilogram of potassium oxide',
			'KPP'=>'kilogram of phosphorus pentoxide (phosphoric anhydride)',
			'KR'=>'kiloroentgen',
			'KSD'=>'kilogram of substance 90 % dry',
			'KSH'=>'kilogram of sodium hydroxide (caustic soda)',
			'KT'=>'kit',
			'KTN'=>'kilotonne',
			'KUR'=>'kilogram of uranium',
			'KVA'=>'kilovolt - ampere',
			'KVR'=>'kilovar',
			'KVT'=>'kilovolt',
			'KW'=>'kilogram per millimetre',
			'KWH'=>'kilowatt hour',
			'KWN'=>'Kilowatt hour per normalized cubic metre',
			'KWO'=>'kilogram of tungsten trioxide',
			'KWS'=>'Kilowatt hour per standard cubic metre',
			'KWT'=>'kilowatt',
			'KWY'=>'kilowatt year',
			'KX'=>'millilitre per kilogram',
			'L10'=>'quart (US liquid) per minute',
			'L11'=>'quart (US liquid) per second',
			'L12'=>'metre per second kelvin',
			'L13'=>'metre per second bar',
			'L14'=>'square metre hour degree Celsius per kilocalorie (international table)',
			'L15'=>'millipascal second per kelvin',
			'L16'=>'millipascal second per bar',
			'L17'=>'milligram per cubic metre kelvin',
			'L18'=>'milligram per cubic metre bar',
			'L19'=>'millilitre per litre',
			'L2'=>'litre per minute',
			'L20'=>'reciprocal cubic millimetre',
			'L21'=>'cubic millimetre per cubic metre',
			'L23'=>'mole per hour',
			'L24'=>'mole per kilogram kelvin',
			'L25'=>'mole per kilogram bar',
			'L26'=>'mole per litre kelvin',
			'L27'=>'mole per litre bar',
			'L28'=>'mole per cubic metre kelvin',
			'L29'=>'mole per cubic metre bar',
			'L30'=>'mole per minute',
			'L31'=>'milliroentgen aequivalent men',
			'L32'=>'nanogram per kilogram',
			'L33'=>'ounce (avoirdupois) per day',
			'L34'=>'ounce (avoirdupois) per hour',
			'L35'=>'ounce (avoirdupois) per minute',
			'L36'=>'ounce (avoirdupois) per second',
			'L37'=>'ounce (avoirdupois) per gallon (UK)',
			'L38'=>'ounce (avoirdupois) per gallon (US)',
			'L39'=>'ounce (avoirdupois) per cubic inch',
			'L40'=>'ounce (avoirdupois)-force',
			'L41'=>'ounce (avoirdupois)-force inch',
			'L42'=>'picosiemens per metre',
			'L43'=>'peck (UK)',
			'L44'=>'peck (UK) per day',
			'L45'=>'peck (UK) per hour',
			'L46'=>'peck (UK) per minute',
			'L47'=>'peck (UK) per second',
			'L48'=>'peck (US dry) per day',
			'L49'=>'peck (US dry) per hour',
			'L50'=>'peck (US dry) per minute',
			'L51'=>'peck (US dry) per second',
			'L52'=>'psi per psi',
			'L53'=>'pint (UK) per day',
			'L54'=>'pint (UK) per hour',
			'L55'=>'pint (UK) per minute',
			'L56'=>'pint (UK) per second',
			'L57'=>'pint (US liquid) per day',
			'L58'=>'pint (US liquid) per hour',
			'L59'=>'pint (US liquid) per minute',
			'L60'=>'pint (US liquid) per second',
			'L63'=>'slug per day',
			'L64'=>'slug per foot second',
			'L65'=>'slug per cubic foot',
			'L66'=>'slug per hour',
			'L67'=>'slug per minute',
			'L68'=>'slug per second',
			'L69'=>'tonne per kelvin',
			'L70'=>'tonne per bar',
			'L71'=>'tonne per day',
			'L72'=>'tonne per day kelvin',
			'L73'=>'tonne per day bar',
			'L74'=>'tonne per hour kelvin',
			'L75'=>'tonne per hour bar',
			'L76'=>'tonne per cubic metre kelvin',
			'L77'=>'tonne per cubic metre bar',
			'L78'=>'tonne per minute',
			'L79'=>'tonne per minute kelvin',
			'L80'=>'tonne per minute bar',
			'L81'=>'tonne per second',
			'L82'=>'tonne per second kelvin',
			'L83'=>'tonne per second bar',
			'L84'=>'ton (UK shipping)',
			'L85'=>'ton long per day',
			'L86'=>'ton (US shipping)',
			'L87'=>'ton short per degree Fahrenheit',
			'L88'=>'ton short per day',
			'L89'=>'ton short per hour degree Fahrenheit',
			'L90'=>'ton short per hour psi',
			'L91'=>'ton short per psi',
			'L92'=>'ton (UK long) per cubic yard',
			'L93'=>'ton (US short) per cubic yard',
			'L94'=>'ton-force (US short)',
			'L95'=>'common year',
			'L96'=>'sidereal year',
			'L98'=>'yard per degree Fahrenheit',
			'L99'=>'yard per psi',
			'LA'=>'pound per cubic inch',
			'LAC'=>'lactose excess percentage',
			'LBR'=>'pound',
			'LBT'=>'troy pound (US)',
			'LD'=>'litre per day',
			'LEF'=>'leaf',
			'LF'=>'linear foot',
			'LH'=>'labour hour',
			'LK'=>'link',
			'LM'=>'linear metre',
			'LN'=>'length',
			'LO'=>'lot  [unit of procurement]',
			'LP'=>'liquid pound',
			'LPA'=>'litre of pure alcohol',
			'LR'=>'layer',
			'LS'=>'lump sum',
			'LTN'=>'ton (UK) or long ton (US)',
			'LTR'=>'litre',
			'LUB'=>'metric ton, lubricating oil',
			'LUM'=>'lumen',
			'LUX'=>'lux',
			'LY'=>'linear yard',
			'M1'=>'milligram per litre',
			'M10'=>'reciprocal cubic yard',
			'M11'=>'cubic yard per degree Fahrenheit',
			'M12'=>'cubic yard per day',
			'M13'=>'cubic yard per hour',
			'M14'=>'cubic yard per psi',
			'M15'=>'cubic yard per minute',
			'M16'=>'cubic yard per second',
			'M17'=>'kilohertz metre',
			'M18'=>'gigahertz metre',
			'M19'=>'Beaufort',
			'M20'=>'reciprocal megakelvin or megakelvin to the power minus one',
			'M21'=>'reciprocal kilovolt - ampere reciprocal hour',
			'M22'=>'millilitre per square centimetre minute',
			'M23'=>'newton per centimetre',
			'M24'=>'ohm kilometre',
			'M25'=>'percent per degree Celsius',
			'M26'=>'gigaohm per metre',
			'M27'=>'megahertz metre',
			'M29'=>'kilogram per kilogram',
			'M30'=>'reciprocal volt - ampere reciprocal second',
			'M31'=>'kilogram per kilometre',
			'M32'=>'pascal second per litre',
			'M33'=>'millimole per litre',
			'M34'=>'newton metre per square metre',
			'M35'=>'millivolt - ampere',
			'M36'=>'30-day month',
			'M37'=>'actual/360',
			'M38'=>'kilometre per second squared',
			'M39'=>'centimetre per second squared',
			'M4'=>'monetary value',
			'M40'=>'yard per second squared',
			'M41'=>'millimetre per second squared',
			'M42'=>'mile (statute mile) per second squared',
			'M43'=>'mil',
			'M44'=>'revolution',
			'M45'=>'degree [unit of angle] per second squared',
			'M46'=>'revolution per minute',
			'M47'=>'circular mil',
			'M48'=>'square mile (based on U.S. survey foot)',
			'M49'=>'chain (based on U.S. survey foot)',
			'M5'=>'microcurie',
			'M50'=>'furlong',
			'M51'=>'foot (U.S. survey)',
			'M52'=>'mile (based on U.S. survey foot)',
			'M53'=>'metre per pascal',
			'M55'=>'metre per radiant',
			'M56'=>'shake',
			'M57'=>'mile per minute',
			'M58'=>'mile per second',
			'M59'=>'metre per second pascal',
			'M60'=>'metre per hour',
			'M61'=>'inch per year',
			'M62'=>'kilometre per second',
			'M63'=>'inch per minute',
			'M64'=>'yard per second',
			'M65'=>'yard per minute',
			'M66'=>'yard per hour',
			'M67'=>'acre-foot (based on U.S. survey foot)',
			'M68'=>'cord (128 ft3)',
			'M69'=>'cubic mile (UK statute)',
			'M7'=>'micro-inch',
			'M70'=>'ton, register',
			'M71'=>'cubic metre per pascal',
			'M72'=>'bel',
			'M73'=>'kilogram per cubic metre pascal',
			'M74'=>'kilogram per pascal',
			'M75'=>'kilopound-force',
			'M76'=>'poundal',
			'M77'=>'kilogram metre per second squared',
			'M78'=>'pond',
			'M79'=>'square foot per hour',
			'M80'=>'stokes per pascal',
			'M81'=>'square centimetre per second',
			'M82'=>'square metre per second pascal',
			'M83'=>'denier',
			'M84'=>'pound per yard',
			'M85'=>'ton, assay',
			'M86'=>'pfund',
			'M87'=>'kilogram per second pascal',
			'M88'=>'tonne per month',
			'M89'=>'tonne per year',
			'M9'=>'million Btu per 1000 cubic foot',
			'M90'=>'kilopound per hour',
			'M91'=>'pound per pound',
			'M92'=>'pound-force foot',
			'M93'=>'newton metre per radian',
			'M94'=>'kilogram metre',
			'M95'=>'poundal foot',
			'M96'=>'poundal inch',
			'M97'=>'dyne metre',
			'M98'=>'kilogram centimetre per second',
			'M99'=>'gram centimetre per second',
			'MAH'=>'megavolt ampere reactive hour',
			'MAL'=>'megalitre',
			'MAM'=>'megametre',
			'MAR'=>'megavar',
			'MAW'=>'megawatt',
			'MBE'=>'thousand standard brick equivalent',
			'MBF'=>'thousand board foot',
			'MBR'=>'millibar',
			'MC'=>'microgram',
			'MCU'=>'millicurie',
			'MD'=>'air dry metric ton',
			'MGM'=>'milligram',
			'MHZ'=>'megahertz',
			'MIK'=>'square mile (statute mile)',
			'MIL'=>'thousand',
			'MIN'=>'minute [unit of time]',
			'MIO'=>'million',
			'MIU'=>'million international unit',
			'MKD'=>'Square Metre Day',
			'MKM'=>'Square Metre Month',
			'MKW'=>'Square Metre Week',
			'MLD'=>'milliard',
			'MLT'=>'millilitre',
			'MMK'=>'square millimetre',
			'MMQ'=>'cubic millimetre',
			'MMT'=>'millimetre',
			'MND'=>'kilogram, dry weight',
			'MNJ'=>'Mega Joule per Normalised cubic Metre',
			'MON'=>'month',
			'MPA'=>'megapascal',
			'MQD'=>'Cubic Metre Day',
			'MQH'=>'cubic metre per hour',
			'MQM'=>'Cubic Metre Month',
			'MQS'=>'cubic metre per second',
			'MQW'=>'Cubic Metre Week',
			'MRD'=>'Metre Day',
			'MRM'=>'Metre Month',
			'MRW'=>'Metre Week',
			'MSK'=>'metre per second squared',
			'MTK'=>'square metre',
			'MTQ'=>'cubic metre',
			'MTR'=>'metre',
			'MTS'=>'metre per second',
			'MTZ'=>'milihertz',
			'MVA'=>'megavolt - ampere',
			'MWH'=>'megawatt hour (1000 kW.h)',
			'N1'=>'pen calorie',
			'N10'=>'pound foot per second',
			'N11'=>'pound inch per second',
			'N12'=>'Pferdestaerke',
			'N13'=>'centimetre of mercury (0 ºC)',
			'N14'=>'centimetre of water (4 ºC)',
			'N15'=>'foot of water (39.2 ºF)',
			'N16'=>'inch of mercury (32 ºF)',
			'N17'=>'inch of mercury (60 ºF)',
			'N18'=>'inch of water (39.2 ºF)',
			'N19'=>'inch of water (60 ºF)',
			'N20'=>'kip per square inch',
			'N21'=>'poundal per square foot',
			'N22'=>'ounce (avoirdupois) per square inch',
			'N23'=>'conventional metre of water',
			'N24'=>'gram per square millimetre',
			'N25'=>'pound per square yard',
			'N26'=>'poundal per square inch',
			'N27'=>'foot to the fourth power',
			'N28'=>'cubic decimetre per kilogram',
			'N29'=>'cubic foot per pound',
			'N3'=>'print point',
			'N30'=>'cubic inch per pound',
			'N31'=>'kilonewton per metre',
			'N32'=>'poundal per inch',
			'N33'=>'pound-force per yard',
			'N34'=>'poundal second per square foot',
			'N35'=>'poise per pascal',
			'N36'=>'newton second per square metre',
			'N37'=>'kilogram per metre second',
			'N38'=>'kilogram per metre minute',
			'N39'=>'kilogram per metre day',
			'N40'=>'kilogram per metre hour',
			'N41'=>'gram per centimetre second',
			'N42'=>'poundal second per square inch',
			'N43'=>'pound per foot minute',
			'N44'=>'pound per foot day',
			'N45'=>'cubic metre per second pascal',
			'N46'=>'foot poundal',
			'N47'=>'inch poundal',
			'N48'=>'watt per square centimetre',
			'N49'=>'watt per square inch',
			'N50'=>'British thermal unit (international table) per square foot hour',
			'N51'=>'British thermal unit (thermochemical) per square foot hour',
			'N52'=>'British thermal unit (thermochemical) per square foot minute',
			'N53'=>'British thermal unit (international table) per square foot second',
			'N54'=>'British thermal unit (thermochemical) per square foot second',
			'N55'=>'British thermal unit (international table) per square inch second',
			'N56'=>'calorie (thermochemical) per square centimetre minute',
			'N57'=>'calorie (thermochemical) per square centimetre second',
			'N58'=>'British thermal unit (international table) per cubic foot',
			'N59'=>'British thermal unit (thermochemical) per cubic foot',
			'N60'=>'British thermal unit (international table) per degree Fahrenheit',
			'N61'=>'British thermal unit (thermochemical) per degree Fahrenheit',
			'N62'=>'British thermal unit (international table) per degree Rankine',
			'N63'=>'British thermal unit (thermochemical) per degree Rankine',
			'N64'=>'British thermal unit (thermochemical) per pound degree Rankine',
			'N65'=>'kilocalorie (international table) per gram kelvin',
			'N66'=>'British thermal unit (39 ºF)',
			'N67'=>'British thermal unit (59 ºF)',
			'N68'=>'British thermal unit (60 ºF)',
			'N69'=>'calorie (20 ºC)',
			'N70'=>'quad (1015 BtuIT)',
			'N71'=>'therm (EC)',
			'N72'=>'therm (U.S.)',
			'N73'=>'British thermal unit (thermochemical) per pound',
			'N74'=>'British thermal unit (international table) per hour square foot degree Fahrenheit',
			'N75'=>'British thermal unit (thermochemical) per hour square foot degree Fahrenheit',
			'N76'=>'British thermal unit (international table) per second square foot degree Fahrenheit',
			'N77'=>'British thermal unit (thermochemical) per second square foot degree Fahrenheit',
			'N78'=>'kilowatt per square metre kelvin',
			'N79'=>'kelvin per pascal',
			'N80'=>'watt per metre degree Celsius',
			'N81'=>'kilowatt per metre kelvin',
			'N82'=>'kilowatt per metre degree Celsius',
			'N83'=>'metre per degree Celcius metre',
			'N84'=>'degree Fahrenheit hour per British thermal unit (international table)',
			'N85'=>'degree Fahrenheit hour per British thermal unit (thermochemical)',
			'N86'=>'degree Fahrenheit second per British thermal unit (international table)',
			'N87'=>'degree Fahrenheit second per British thermal unit (thermochemical)',
			'N88'=>'degree Fahrenheit hour square foot per British thermal unit (international table) inch',
			'N89'=>'degree Fahrenheit hour square foot per British thermal unit (thermochemical) inch',
			'N90'=>'kilofarad',
			'N91'=>'reciprocal joule',
			'N92'=>'picosiemens',
			'N93'=>'ampere per pascal',
			'N94'=>'franklin',
			'N95'=>'ampere minute',
			'N96'=>'biot',
			'N97'=>'gilbert',
			'N98'=>'volt per pascal',
			'N99'=>'picovolt',
			'NA'=>'milligram per kilogram',
			'NAR'=>'number of articles',
			'NCL'=>'number of cells',
			'NEW'=>'newton',
			'NF'=>'message',
			'NIL'=>'nil',
			'NIU'=>'number of international units',
			'NL'=>'load',
			'NM3'=>'Normalised cubic metre',
			'NMI'=>'nautical mile',
			'NMP'=>'number of packs',
			'NPT'=>'number of parts',
			'NT'=>'net ton',
			'NTU'=>'Nephelometric turbidity unit',
			'NU'=>'newton metre',
			'NX'=>'part per thousand',
			'OA'=>'panel',
			'ODE'=>'ozone depletion equivalent',
			'ODG'=>'ODS Grams',
			'ODK'=>'ODS Kilograms',
			'ODM'=>'ODS Milligrams',
			'OHM'=>'ohm',
			'ON'=>'ounce per square yard',
			'ONZ'=>'ounce (avoirdupois)',
			'OPM'=>'oscillations per minute',
			'OT'=>'overtime hour',
			'OZA'=>'fluid ounce (US)',
			'OZI'=>'fluid ounce (UK)',
			'P1'=>'percent',
			'P10'=>'coulomb per metre',
			'P11'=>'kiloweber',
			'P12'=>'gamma',
			'P13'=>'kilotesla',
			'P14'=>'joule per second',
			'P15'=>'joule per minute',
			'P16'=>'joule per hour',
			'P17'=>'joule per day',
			'P18'=>'kilojoule per second',
			'P19'=>'kilojoule per minute',
			'P2'=>'pound per foot',
			'P20'=>'kilojoule per hour',
			'P21'=>'kilojoule per day',
			'P22'=>'nanoohm',
			'P23'=>'ohm circular-mil per foot',
			'P24'=>'kilohenry',
			'P25'=>'lumen per square foot',
			'P26'=>'phot',
			'P27'=>'footcandle',
			'P28'=>'candela per square inch',
			'P29'=>'footlambert',
			'P30'=>'lambert',
			'P31'=>'stilb',
			'P32'=>'candela per square foot',
			'P33'=>'kilocandela',
			'P34'=>'millicandela',
			'P35'=>'Hefner-Kerze',
			'P36'=>'international candle',
			'P37'=>'British thermal unit (international table) per square foot',
			'P38'=>'British thermal unit (thermochemical) per square foot',
			'P39'=>'calorie (thermochemical) per square centimetre',
			'P40'=>'langley',
			'P41'=>'decade (logarithmic)',
			'P42'=>'pascal squared second',
			'P43'=>'bel per metre',
			'P44'=>'pound mole',
			'P45'=>'pound mole per second',
			'P46'=>'pound mole per minute',
			'P47'=>'kilomole per kilogram',
			'P48'=>'pound mole per pound',
			'P49'=>'newton square metre per ampere',
			'P5'=>'five pack',
			'P50'=>'weber metre',
			'P51'=>'mol per kilogram pascal',
			'P52'=>'mol per cubic metre pascal',
			'P53'=>'unit pole',
			'P54'=>'milligray per second',
			'P55'=>'microgray per second',
			'P56'=>'nanogray per second',
			'P57'=>'gray per minute',
			'P58'=>'milligray per minute',
			'P59'=>'microgray per minute',
			'P60'=>'nanogray per minute',
			'P61'=>'gray per hour',
			'P62'=>'milligray per hour',
			'P63'=>'microgray per hour',
			'P64'=>'nanogray per hour',
			'P65'=>'sievert per second',
			'P66'=>'millisievert per second',
			'P67'=>'microsievert per second',
			'P68'=>'nanosievert per second',
			'P69'=>'rem per second',
			'P70'=>'sievert per hour',
			'P71'=>'millisievert per hour',
			'P72'=>'microsievert per hour',
			'P73'=>'nanosievert per hour',
			'P74'=>'sievert per minute',
			'P75'=>'millisievert per minute',
			'P76'=>'microsievert per minute',
			'P77'=>'nanosievert per minute',
			'P78'=>'reciprocal square inch',
			'P79'=>'pascal square metre per kilogram',
			'P80'=>'millipascal per metre',
			'P81'=>'kilopascal per metre',
			'P82'=>'hectopascal per metre',
			'P83'=>'standard atmosphere per metre',
			'P84'=>'technical atmosphere per metre',
			'P85'=>'torr per metre',
			'P86'=>'psi per inch',
			'P87'=>'cubic metre per second square metre',
			'P88'=>'rhe',
			'P89'=>'pound-force foot per inch',
			'P90'=>'pound-force inch per inch',
			'P91'=>'perm (0 ºC)',
			'P92'=>'perm (23 ºC)',
			'P93'=>'byte per second',
			'P94'=>'kilobyte per second',
			'P95'=>'megabyte per second',
			'P96'=>'reciprocal volt',
			'P97'=>'reciprocal radian',
			'P98'=>'pascal to the power sum of stoichiometric numbers',
			'P99'=>'mole per cubiv metre to the power sum of stoichiometric numbers',
			'PAL'=>'pascal',
			'PD'=>'pad',
			'PFL'=>'proof litre',
			'PGL'=>'proof gallon',
			'PI'=>'pitch',
			'PLA'=>'degree Plato',
			'PO'=>'pound per inch of length',
			'PQ'=>'page per inch',
			'PR'=>'pair',
			'PS'=>'pound-force per square inch',
			'PTD'=>'dry pint (US)',
			'PTI'=>'pint (UK)',
			'PTL'=>'liquid pint (US)',
			'PTN'=>'portion',
			'Q10'=>'joule per tesla',
			'Q11'=>'erlang',
			'Q12'=>'octet',
			'Q13'=>'octet per second',
			'Q14'=>'shannon',
			'Q15'=>'hartley',
			'Q16'=>'natural unit of information',
			'Q17'=>'shannon per second',
			'Q18'=>'hartley per second',
			'Q19'=>'natural unit of information per second',
			'Q20'=>'second per kilogramm',
			'Q21'=>'watt square metre',
			'Q22'=>'second per radian cubic metre',
			'Q23'=>'weber to the power minus one',
			'Q24'=>'reciprocal inch',
			'Q25'=>'dioptre',
			'Q26'=>'one per one',
			'Q27'=>'newton metre per metre',
			'Q28'=>'kilogram per square metre pascal second',
			'Q29'=>'microgram per hectogram',
			'Q3'=>'meal',
			'Q30'=>'pH (potential of Hydrogen)',
			'Q31'=>'kilojoule per gram',
			'Q32'=>'femtolitre',
			'Q33'=>'picolitre',
			'Q34'=>'nanolitre',
			'Q35'=>'megawatts per minute',
			'Q36'=>'square metre per cubic metre',
			'Q37'=>'Standard cubic metre per day',
			'Q38'=>'Standard cubic metre per hour',
			'Q39'=>'Normalized cubic metre per day',
			'Q40'=>'Normalized cubic metre per hour',
			'Q41'=>'Joule per normalised cubic metre',
			'Q42'=>'Joule per standard cubic metre',
			'QA'=>'page - facsimile',
			'QAN'=>'quarter (of a year)',
			'QB'=>'page - hardcopy',
			'QR'=>'quire',
			'QTD'=>'dry quart (US)',
			'QTI'=>'quart (UK)',
			'QTL'=>'liquid quart (US)',
			'QTR'=>'quarter (UK)',
			'R1'=>'pica',
			'R9'=>'thousand cubic metre',
			'RH'=>'running or operating hour',
			'RM'=>'ream',
			'ROM'=>'room',
			'RP'=>'pound per ream',
			'RPM'=>'revolutions per minute',
			'RPS'=>'revolutions per second',
			'RT'=>'revenue ton mile',
			'S3'=>'square foot per second',
			'S4'=>'square metre per second',
			'SAN'=>'half year (6 months)',
			'SCO'=>'score',
			'SCR'=>'scruple',
			'SEC'=>'second [unit of time]',
			'SET'=>'set',
			'SG'=>'segment',
			'SIE'=>'siemens',
			'SM3'=>'Standard cubic metre',
			'SMI'=>'mile (statute mile)',
			'SQ'=>'square',
			'SQR'=>'square, roofing',
			'SR'=>'strip',
			'STC'=>'stick',
			'STI'=>'stone (UK)',
			'STK'=>'stick, cigarette',
			'STL'=>'standard litre',
			'STN'=>'ton (US) or short ton (UK/US)',
			'STW'=>'straw',
			'SW'=>'skein',
			'SX'=>'shipment',
			'SYR'=>'syringe',
			'T0'=>'telecommunication line in service',
			'T3'=>'thousand piece',
			'TAH'=>'kiloampere hour (thousand ampere hour)',
			'TAN'=>'total acid number',
			'TI'=>'thousand square inch',
			'TIC'=>'metric ton, including container',
			'TIP'=>'metric ton, including inner packaging',
			'TKM'=>'tonne kilometre',
			'TMS'=>'kilogram of imported meat, less offal',
			'TNE'=>'tonne (metric ton)',
			'TP'=>'ten pack',
			'TPI'=>'teeth per inch',
			'TPR'=>'ten pair',
			'TQD'=>'thousand cubic metre per day',
			'TRL'=>'trillion (EUR)',
			'TST'=>'ten set',
			'TTS'=>'ten thousand sticks',
			'U1'=>'treatment',
			'U2'=>'tablet',
			'UB'=>'telecommunication line in service average',
			'UC'=>'telecommunication port',
			'VA'=>'volt - ampere per kilogram',
			'VLT'=>'volt',
			'VP'=>'percent volume',
			'W2'=>'wet kilo',
			'WA'=>'watt per kilogram',
			'WB'=>'wet pound',
			'WCD'=>'cord',
			'WE'=>'wet ton',
			'WEB'=>'weber',
			'WEE'=>'week',
			'WG'=>'wine gallon',
			'WHR'=>'watt hour',
			'WM'=>'working month',
			'WSD'=>'standard',
			'WTT'=>'watt',
			'X1'=>'Gunter\'s chain',
			'YDK'=>'square yard',
			'YDQ'=>'cubic yard',
			'YRD'=>'yard',
			'Z11'=>'hanging container',
			'Z9'=>'nanomole',
			'ZP'=>'page',
			'ZZ'=>'mutually defined',
			'X1A'=>'Drum, steel',
			'X1B'=>'Drum, aluminium',
			'X1D'=>'Drum, plywood',
			'X1F'=>'Container, flexible',
			'X1G'=>'Drum, fibre',
			'X1W'=>'Drum, wooden',
			'X2C'=>'Barrel, wooden',
			'X3A'=>'Jerrican, steel',
			'X3H'=>'Jerrican, plastic',
			'X43'=>'Bag, super bulk',
			'X44'=>'Bag, polybag',
			'X4A'=>'Box, steel',
			'X4B'=>'Box, aluminium',
			'X4C'=>'Box, natural wood',
			'X4D'=>'Box, plywood',
			'X4F'=>'Box, reconstituted wood',
			'X4G'=>'Box, fibreboard',
			'X4H'=>'Box, plastic',
			'X5H'=>'Bag, woven plastic',
			'X5L'=>'Bag, textile',
			'X5M'=>'Bag, paper',
			'X6H'=>'Composite packaging, plastic receptacle',
			'X6P'=>'Composite packaging, glass receptacle',
			'X7A'=>'Case, car',
			'X7B'=>'Case, wooden',
			'X8A'=>'Pallet, wooden',
			'X8B'=>'Crate, wooden',
			'X8C'=>'Bundle, wooden',
			'XAA'=>'Intermediate bulk container, rigid plastic',
			'XAB'=>'Receptacle, fibre',
			'XAC'=>'Receptacle, paper',
			'XAD'=>'Receptacle, wooden',
			'XAE'=>'Aerosol',
			'XAF'=>'Pallet, modular, collars 80cms * 60cms',
			'XAG'=>'Pallet, shrinkwrapped',
			'XAH'=>'Pallet, 100cms * 110cms',
			'XAI'=>'Clamshell',
			'XAJ'=>'Cone',
			'XAL'=>'Ball',
			'XAM'=>'Ampoule, non-protected',
			'XAP'=>'Ampoule, protected',
			'XAT'=>'Atomizer',
			'XAV'=>'Capsule',
			'XB4'=>'Belt',
			'XBA'=>'Barrel',
			'XBB'=>'Bobbin',
			'XBC'=>'Bottlecrate / bottlerack',
			'XBD'=>'Board',
			'XBE'=>'Bundle',
			'XBF'=>'Balloon, non-protected',
			'XBG'=>'Bag',
			'XBH'=>'Bunch',
			'XBI'=>'Bin',
			'XBJ'=>'Bucket',
			'XBK'=>'Basket',
			'XBL'=>'Bale, compressed',
			'XBM'=>'Basin',
			'XBN'=>'Bale, non-compressed',
			'XBO'=>'Bottle, non-protected, cylindrical',
			'XBP'=>'Balloon, protected',
			'XBQ'=>'Bottle, protected cylindrical',
			'XBR'=>'Bar',
			'XBS'=>'Bottle, non-protected, bulbous',
			'XBT'=>'Bolt',
			'XBU'=>'Butt',
			'XBV'=>'Bottle, protected bulbous',
			'XBW'=>'Box, for liquids',
			'XBX'=>'Box',
			'XBY'=>'Board, in bundle/bunch/truss',
			'XBZ'=>'Bars, in bundle/bunch/truss',
			'XCA'=>'Can, rectangular',
			'XCB'=>'Crate, beer',
			'XCC'=>'Churn',
			'XCD'=>'Can, with handle and spout',
			'XCE'=>'Creel',
			'XCF'=>'Coffer',
			'XCG'=>'Cage',
			'XCH'=>'Chest',
			'XCI'=>'Canister',
			'XCJ'=>'Coffin',
			'XCK'=>'Cask',
			'XCL'=>'Coil',
			'XCM'=>'Card',
			'XCN'=>'Container, not otherwise specified as transport equipment',
			'XCO'=>'Carboy, non-protected',
			'XCP'=>'Carboy, protected',
			'XCQ'=>'Cartridge',
			'XCR'=>'Crate',
			'XCS'=>'Case',
			'XCT'=>'Carton',
			'XCU'=>'Cup',
			'XCV'=>'Cover',
			'XCW'=>'Cage, roll',
			'XCX'=>'Can, cylindrical',
			'XCY'=>'Cylinder',
			'XCZ'=>'Canvas',
			'XDA'=>'Crate, multiple layer, plastic',
			'XDB'=>'Crate, multiple layer, wooden',
			'XDC'=>'Crate, multiple layer, cardboard',
			'XDG'=>'Cage, Commonwealth Handling Equipment Pool  (CHEP)',
			'XDH'=>'Box, Commonwealth Handling Equipment Pool (CHEP), Eurobox',
			'XDI'=>'Drum, iron',
			'XDJ'=>'Demijohn, non-protected',
			'XDK'=>'Crate, bulk, cardboard',
			'XDL'=>'Crate, bulk, plastic',
			'XDM'=>'Crate, bulk, wooden',
			'XDN'=>'Dispenser',
			'XDP'=>'Demijohn, protected',
			'XDR'=>'Drum',
			'XDS'=>'Tray, one layer no cover, plastic',
			'XDT'=>'Tray, one layer no cover, wooden',
			'XDU'=>'Tray, one layer no cover, polystyrene',
			'XDV'=>'Tray, one layer no cover, cardboard',
			'XDW'=>'Tray, two layers no cover, plastic tray',
			'XDX'=>'Tray, two layers no cover, wooden',
			'XDY'=>'Tray, two layers no cover, cardboard',
			'XEC'=>'Bag, plastic',
			'XED'=>'Case, with pallet base',
			'XEE'=>'Case, with pallet base, wooden',
			'XEF'=>'Case, with pallet base, cardboard',
			'XEG'=>'Case, with pallet base, plastic',
			'XEH'=>'Case, with pallet base, metal',
			'XEI'=>'Case, isothermic',
			'XEN'=>'Envelope',
			'XFB'=>'Flexibag',
			'XFC'=>'Crate, fruit',
			'XFD'=>'Crate, framed',
			'XFE'=>'Flexitank',
			'XFI'=>'Firkin',
			'XFL'=>'Flask',
			'XFO'=>'Footlocker',
			'XFP'=>'Filmpack',
			'XFR'=>'Frame',
			'XFT'=>'Foodtainer',
			'XFW'=>'Cart, flatbed',
			'XFX'=>'Bag, flexible container',
			'XGB'=>'Bottle, gas',
			'XGI'=>'Girder',
			'XGL'=>'Container, gallon',
			'XGR'=>'Receptacle, glass',
			'XGU'=>'Tray, containing horizontally stacked flat items',
			'XGY'=>'Bag, gunny',
			'XGZ'=>'Girders, in bundle/bunch/truss',
			'XHA'=>'Basket, with handle, plastic',
			'XHB'=>'Basket, with handle, wooden',
			'XHC'=>'Basket, with handle, cardboard',
			'XHG'=>'Hogshead',
			'XHN'=>'Hanger',
			'XHR'=>'Hamper',
			'XIA'=>'Package, display, wooden',
			'XIB'=>'Package, display, cardboard',
			'XIC'=>'Package, display, plastic',
			'XID'=>'Package, display, metal',
			'XIE'=>'Package, show',
			'XIF'=>'Package, flow',
			'XIG'=>'Package, paper wrapped',
			'XIH'=>'Drum, plastic',
			'XIK'=>'Package, cardboard, with bottle grip-holes',
			'XIL'=>'Tray, rigid, lidded stackable (CEN TS 14482:2002)',
			'XIN'=>'Ingot',
			'XIZ'=>'Ingots, in bundle/bunch/truss',
			'XJB'=>'Bag, jumbo',
			'XJC'=>'Jerrican, rectangular',
			'XJG'=>'Jug',
			'XJR'=>'Jar',
			'XJT'=>'Jutebag',
			'XJY'=>'Jerrican, cylindrical',
			'XKG'=>'Keg',
			'XKI'=>'Kit',
			'XLE'=>'Luggage',
			'XLG'=>'Log',
			'XLT'=>'Lot',
			'XLU'=>'Lug',
			'XLV'=>'Liftvan',
			'XLZ'=>'Logs, in bundle/bunch/truss',
			'XMA'=>'Crate, metal',
			'XMB'=>'Bag, multiply',
			'XMC'=>'Crate, milk',
			'XME'=>'Container, metal',
			'XMR'=>'Receptacle, metal',
			'XMS'=>'Sack, multi-wall',
			'XMT'=>'Mat',
			'XMW'=>'Receptacle, plastic wrapped',
			'XMX'=>'Matchbox',
			'XNA'=>'Not available',
			'XNE'=>'Unpacked or unpackaged',
			'XNF'=>'Unpacked or unpackaged, single unit',
			'XNG'=>'Unpacked or unpackaged, multiple units',
			'XNS'=>'Nest',
			'XNT'=>'Net',
			'XNU'=>'Net, tube, plastic',
			'XNV'=>'Net, tube, textile',
			'XO1'=>'Two sided cage on wheels with fixing strap',
			'XO2'=>'Trolley',
			'XO3'=>'Oneway pallet ISO 0 - 1/2 EURO Pallet',
			'XO4'=>'Oneway pallet ISO 1 - 1/1 EURO Pallet',
			'XO5'=>'Oneway pallet ISO 2 - 2/1 EURO Pallet',
			'XO6'=>'Pallet with exceptional dimensions',
			'XO7'=>'Wooden pallet  40 cm x 80 cm',
			'XO8'=>'Plastic pallet SRS 60 cm x 80 cm',
			'XO9'=>'Plastic pallet SRS 80 cm x 120 cm',
			'XOA'=>'Pallet, CHEP 40 cm x 60 cm',
			'XOB'=>'Pallet, CHEP 80 cm x 120 cm',
			'XOC'=>'Pallet, CHEP 100 cm x 120 cm',
			'XOD'=>'Pallet, AS 4068-1993',
			'XOE'=>'Pallet, ISO T11',
			'XOF'=>'Platform, unspecified weight or dimension',
			'XOG'=>'Pallet ISO 0 - 1/2 EURO Pallet',
			'XOH'=>'Pallet ISO 1 - 1/1 EURO Pallet',
			'XOI'=>'Pallet ISO 2 – 2/1 EURO Pallet',
			'XOJ'=>'1/4 EURO Pallet',
			'XOK'=>'Block',
			'XOL'=>'1/8 EURO Pallet',
			'XOM'=>'Synthetic pallet ISO 1',
			'XON'=>'Synthetic pallet ISO 2',
			'XOP'=>'Wholesaler pallet',
			'XOQ'=>'Pallet 80 X 100 cm',
			'XOR'=>'Pallet 60 X 100 cm',
			'XOS'=>'Oneway pallet',
			'XOT'=>'Octabin',
			'XOU'=>'Container, outer',
			'XOV'=>'Returnable pallet',
			'XOW'=>'Large bag, pallet sized',
			'XOX'=>'A wheeled pallet with raised rim (81 x 67 x 135)',
			'XOY'=>'A Wheeled pallet with raised rim (81 x 72 x 135)',
			'XOZ'=>'Wheeled pallet with raised rim ( 81 x 60 x 16)',
			'XP1'=>'CHEP pallet 60 cm x 80 cm',
			'XP2'=>'Pan',
			'XP3'=>'LPR pallet 60 cm x 80 cm',
			'XP4'=>'LPR pallet 80 cm x 120 cm',
			'XPA'=>'Packet',
			'XPB'=>'Pallet, box Combined open-ended box and pallet',
			'XPC'=>'Parcel',
			'XPD'=>'Pallet, modular, collars 80cms * 100cms',
			'XPE'=>'Pallet, modular, collars 80cms * 120cms',
			'XPF'=>'Pen',
			'XPG'=>'Plate',
			'XPH'=>'Pitcher',
			'XPI'=>'Pipe',
			'XPJ'=>'Punnet',
			'XPK'=>'Package',
			'XPL'=>'Pail',
			'XPN'=>'Plank',
			'XPO'=>'Pouch',
			'XPP'=>'Piece',
			'XPR'=>'Receptacle, plastic',
			'XPT'=>'Pot',
			'XPU'=>'Tray',
			'XPV'=>'Pipes, in bundle/bunch/truss',
			'XPX'=>'Pallet',
			'XPY'=>'Plates, in bundle/bunch/truss',
			'XPZ'=>'Planks, in bundle/bunch/truss',
			'XQA'=>'Drum, steel, non-removable head',
			'XQB'=>'Drum, steel, removable head',
			'XQC'=>'Drum, aluminium, non-removable head',
			'XQD'=>'Drum, aluminium, removable head',
			'XQF'=>'Drum, plastic, non-removable head',
			'XQG'=>'Drum, plastic, removable head',
			'XQH'=>'Barrel, wooden, bung type',
			'XQJ'=>'Barrel, wooden, removable head',
			'XQK'=>'Jerrican, steel, non-removable head',
			'XQL'=>'Jerrican, steel, removable head',
			'XQM'=>'Jerrican, plastic, non-removable head',
			'XQN'=>'Jerrican, plastic, removable head',
			'XQP'=>'Box, wooden, natural wood, ordinary',
			'XQQ'=>'Box, wooden, natural wood, with sift proof walls',
			'XQR'=>'Box, plastic, expanded',
			'XQS'=>'Box, plastic, solid',
			'XRD'=>'Rod',
			'XRG'=>'Ring',
			'XRJ'=>'Rack, clothing hanger',
			'XRK'=>'Rack',
			'XRL'=>'Reel',
			'XRO'=>'Roll',
			'XRT'=>'Rednet',
			'XRZ'=>'Rods, in bundle/bunch/truss',
			'XSA'=>'Sack',
			'XSB'=>'Slab',
			'XSC'=>'Crate, shallow',
			'XSD'=>'Spindle',
			'XSE'=>'Sea-chest',
			'XSH'=>'Sachet',
			'XSI'=>'Skid',
			'XSK'=>'Case, skeleton',
			'XSL'=>'Slipsheet',
			'XSM'=>'Sheetmetal',
			'XSO'=>'Spool',
			'XSP'=>'Sheet, plastic wrapping',
			'XSS'=>'Case, steel',
			'XST'=>'Sheet',
			'XSU'=>'Suitcase',
			'XSV'=>'Envelope, steel',
			'XSW'=>'Shrinkwrapped',
			'XSX'=>'Set',
			'XSY'=>'Sleeve',
			'XSZ'=>'Sheets, in bundle/bunch/truss',
			'XT1'=>'Tablet',
			'XTB'=>'Tub',
			'XTC'=>'Tea-chest',
			'XTD'=>'Tube, collapsible',
			'XTE'=>'Tyre',
			'XTG'=>'Tank container, generic',
			'XTI'=>'Tierce',
			'XTK'=>'Tank, rectangular',
			'XTL'=>'Tub, with lid',
			'XTN'=>'Tin',
			'XTO'=>'Tun',
			'XTR'=>'Trunk',
			'XTS'=>'Truss',
			'XTT'=>'Bag, tote',
			'XTU'=>'Tube',
			'XTV'=>'Tube, with nozzle',
			'XTW'=>'Pallet, triwall',
			'XTY'=>'Tank, cylindrical',
			'XTZ'=>'Tubes, in bundle/bunch/truss',
			'XUC'=>'Uncaged',
			'XUN'=>'Unit',
			'XVA'=>'Vat',
			'XVG'=>'Bulk, gas (at 1031 mbar and 15°C)',
			'XVI'=>'Vial',
			'XVK'=>'Vanpack',
			'XVL'=>'Bulk, liquid',
			'XVN'=>'Vehicle',
			'XVO'=>'Bulk, solid, large particles (“nodules”)',
			'XVP'=>'Vacuum-packed',
			'XVQ'=>'Bulk, liquefied gas (at abnormal temperature/pressure)',
			'XVR'=>'Bulk, solid, granular particles (“grains”)',
			'XVS'=>'Bulk, scrap metal',
			'XVY'=>'Bulk, solid, fine particles (“powders”)',
			'XWA'=>'Intermediate bulk container',
			'XWB'=>'Wickerbottle',
			'XWC'=>'Intermediate bulk container, steel',
			'XWD'=>'Intermediate bulk container, aluminium',
			'XWF'=>'Intermediate bulk container, metal',
			'XWG'=>'Intermediate bulk container, steel, pressurised > 10 kpa',
			'XWH'=>'Intermediate bulk container, aluminium, pressurised > 10 kpa',
			'XWJ'=>'Intermediate bulk container, metal, pressure 10 kpa',
			'XWK'=>'Intermediate bulk container, steel, liquid',
			'XWL'=>'Intermediate bulk container, aluminium, liquid',
			'XWM'=>'Intermediate bulk container, metal, liquid',
			'XWN'=>'Intermediate bulk container, woven plastic, without coat/liner',
			'XWP'=>'Intermediate bulk container, woven plastic, coated',
			'XWQ'=>'Intermediate bulk container, woven plastic, with liner',
			'XWR'=>'Intermediate bulk container, woven plastic, coated and liner',
			'XWS'=>'Intermediate bulk container, plastic film',
			'XWT'=>'Intermediate bulk container, textile with out coat/liner',
			'XWU'=>'Intermediate bulk container, natural wood, with inner liner',
			'XWV'=>'Intermediate bulk container, textile, coated',
			'XWW'=>'Intermediate bulk container, textile, with liner',
			'XWX'=>'Intermediate bulk container, textile, coated and liner',
			'XWY'=>'Intermediate bulk container, plywood, with inner liner',
			'XWZ'=>'Intermediate bulk container, reconstituted wood, with inner liner',
			'XXA'=>'Bag, woven plastic, without inner coat/liner',
			'XXB'=>'Bag, woven plastic, sift proof',
			'XXC'=>'Bag, woven plastic, water resistant',
			'XXD'=>'Bag, plastics film',
			'XXF'=>'Bag, textile, without inner coat/liner',
			'XXG'=>'Bag, textile, sift proof',
			'XXH'=>'Bag, textile, water resistant',
			'XXJ'=>'Bag, paper, multi-wall',
			'XXK'=>'Bag, paper, multi-wall, water resistant',
			'XYA'=>'Composite packaging, plastic receptacle in steel drum',
			'XYB'=>'Composite packaging, plastic receptacle in steel crate box',
			'XYC'=>'Composite packaging, plastic receptacle in aluminium drum',
			'XYD'=>'Composite packaging, plastic receptacle in aluminium crate',
			'XYF'=>'Composite packaging, plastic receptacle in wooden box',
			'XYG'=>'Composite packaging, plastic receptacle in plywood drum',
			'XYH'=>'Composite packaging, plastic receptacle in plywood box',
			'XYJ'=>'Composite packaging, plastic receptacle in fibre drum',
			'XYK'=>'Composite packaging, plastic receptacle in fibreboard box',
			'XYL'=>'Composite packaging, plastic receptacle in plastic drum',
			'XYM'=>'Composite packaging, plastic receptacle in solid plastic box',
			'XYN'=>'Composite packaging, glass receptacle in steel drum',
			'XYP'=>'Composite packaging, glass receptacle in steel crate box',
			'XYQ'=>'Composite packaging, glass receptacle in aluminium drum',
			'XYR'=>'Composite packaging, glass receptacle in aluminium crate',
			'XYS'=>'Composite packaging, glass receptacle in wooden box',
			'XYT'=>'Composite packaging, glass receptacle in plywood drum',
			'XYV'=>'Composite packaging, glass receptacle in wickerwork hamper',
			'XYW'=>'Composite packaging, glass receptacle in fibre drum',
			'XYX'=>'Composite packaging, glass receptacle in fibreboard box',
			'XYY'=>'Composite packaging, glass receptacle in expandable plastic pack',
			'XYZ'=>'Composite packaging, glass receptacle in solid plastic pack',
			'XZA'=>'Intermediate bulk container, paper, multi-wall',
			'XZB'=>'Bag, large',
			'XZC'=>'Intermediate bulk container, paper, multi-wall, water resistant',
			'XZD'=>'Intermediate bulk container, rigid plastic, with structural equipment, solids',
			'XZF'=>'Intermediate bulk container, rigid plastic, freestanding, solids',
			'XZG'=>'Intermediate bulk container, rigid plastic, with structural equipment, pressurised',
			'XZH'=>'Intermediate bulk container, rigid plastic, freestanding, pressurised',
			'XZJ'=>'Intermediate bulk container, rigid plastic, with structural equipment, liquids',
			'XZK'=>'Intermediate bulk container, rigid plastic, freestanding, liquids',
			'XZL'=>'Intermediate bulk container, composite, rigid plastic, solids',
			'XZM'=>'Intermediate bulk container, composite, flexible plastic, solids',
			'XZN'=>'Intermediate bulk container, composite, rigid plastic, pressurised',
			'XZP'=>'Intermediate bulk container, composite, flexible plastic, pressurised',
			'XZQ'=>'Intermediate bulk container, composite, rigid plastic, liquids',
			'XZR'=>'Intermediate bulk container, composite, flexible plastic, liquids',
			'XZS'=>'Intermediate bulk container, composite',
			'XZT'=>'Intermediate bulk container, fibreboard',
			'XZU'=>'Intermediate bulk container, flexible',
			'XZV'=>'Intermediate bulk container, metal, other than steel',
			'XZW'=>'Intermediate bulk container, natural wood',
			'XZX'=>'Intermediate bulk container, plywood',
			'XZY'=>'Intermediate bulk container, reconstituted wood',
			'XZZ'=>'Mutually defined'
		);

		if(!strlen($code))
			return $a;
		else if(isset($a[$code]))
			return $a[$code];
		else
			return '['.$code.' !]';
	}

	public function fxcCategoryCode($code='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$code=trim((string)$code);
		if(strlen($code))
			$code=strtoupper(substr($code,0,1));

		$a=array(
			'S'=>'Standard rate',
			'Z'=>'Zero rated goods',
			'E'=>'Exempt from tax',
			'AE'=>'VAT Reverse charge',
			'K'=>'VAT exempt for EEA intra-community supply of goods and services',
			'G'=>'Free export item, tax not charged',
			'O'=>'Service outside scope of tax',
			'L'=>'Canary Islands general indirect tax',
			'M'=>'Tax for production, services and importation in Ceuta and Melilla'
		);

		if(!strlen($code))
			return $a;
		else if(isset($a[$code]))
			return $a[$code];
		else
			return '['.$code.' !]';
	}

	public function fxcCountryID($code='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$code=trim((string)$code);
		if(strlen($code))
			$code=strtoupper(substr($code,0,2));

		$a=array(
			'AD'=>'Andorra',
			'AE'=>'United Arab Emirates (the)',
			'AF'=>'Afghanistan',
			'AG'=>'Antigua and Barbuda',
			'AI'=>'Anguilla',
			'AL'=>'Albania',
			'AM'=>'Armenia',
			'AO'=>'Angola',
			'AQ'=>'Antarctica',
			'AR'=>'Argentina',
			'AS'=>'American Samoa',
			'AT'=>'Austria',
			'AU'=>'Australia',
			'AW'=>'Aruba',
			'AX'=>'Åland Islands',
			'AZ'=>'Azerbaijan',
			'BA'=>'Bosnia and Herzegovina',
			'BB'=>'Barbados',
			'BD'=>'Bangladesh',
			'BE'=>'Belgium',
			'BF'=>'Burkina Faso',
			'BG'=>'Bulgaria',
			'BH'=>'Bahrain',
			'BI'=>'Burundi',
			'BJ'=>'Benin',
			'BL'=>'Saint Barthélemy',
			'BM'=>'Bermuda',
			'BN'=>'Brunei Darussalam',
			'BO'=>'Bolivia (Plurinational State of)',
			'BQ'=>'Bonaire, Sint Eustatius and Saba',
			'BR'=>'Brazil',
			'BS'=>'Bahamas (the)',
			'BT'=>'Bhutan',
			'BV'=>'Bouvet Island',
			'BW'=>'Botswana',
			'BY'=>'Belarus',
			'BZ'=>'Belize',
			'CA'=>'Canada',
			'CC'=>'Cocos (Keeling) Islands (the)',
			'CD'=>'Congo (the Democratic Republic of the)',
			'CF'=>'Central African Republic (the)',
			'CG'=>'Congo (the)',
			'CH'=>'Switzerland',
			'CI'=>'Côte d\'Ivoire',
			'CK'=>'Cook Islands (the)',
			'CL'=>'Chile',
			'CM'=>'Cameroon',
			'CN'=>'China',
			'CO'=>'Colombia',
			'CR'=>'Costa Rica',
			'CU'=>'Cuba',
			'CV'=>'Cabo Verde',
			'CW'=>'Curaçao',
			'CX'=>'Christmas Island',
			'CY'=>'Cyprus',
			'CZ'=>'Czechia',
			'DE'=>'Germany',
			'DJ'=>'Djibouti',
			'DK'=>'Denmark',
			'DM'=>'Dominica',
			'DO'=>'Dominican Republic (the)',
			'DZ'=>'Algeria',
			'EC'=>'Ecuador',
			'EE'=>'Estonia',
			'EG'=>'Egypt',
			'EH'=>'Western Sahara*',
			'ER'=>'Eritrea',
			'ES'=>'Spain',
			'ET'=>'Ethiopia',
			'FI'=>'Finland',
			'FJ'=>'Fiji',
			'FK'=>'Falkland Islands (the) [Malvinas]',
			'FM'=>'Micronesia (Federated States of)',
			'FO'=>'Faroe Islands (the)',
			'FR'=>'France',
			'GA'=>'Gabon',
			'GB'=>'United Kingdom of Great Britain and Northern Ireland (the)',
			'GD'=>'Grenada',
			'GE'=>'Georgia',
			'GF'=>'French Guiana',
			'GG'=>'Guernsey',
			'GH'=>'Ghana',
			'GI'=>'Gibraltar',
			'GL'=>'Greenland',
			'GM'=>'Gambia (the)',
			'GN'=>'Guinea',
			'GP'=>'Guadeloupe',
			'GQ'=>'Equatorial Guinea',
			'GR'=>'Greece',
			'GS'=>'South Georgia and the South Sandwich Islands',
			'GT'=>'Guatemala',
			'GU'=>'Guam',
			'GW'=>'Guinea-Bissau',
			'GY'=>'Guyana',
			'HK'=>'Hong Kong',
			'HM'=>'Heard Island and McDonald Islands',
			'HN'=>'Honduras',
			'HR'=>'Croatia',
			'HT'=>'Haiti',
			'HU'=>'Hungary',
			'ID'=>'Indonesia',
			'IE'=>'Ireland',
			'IL'=>'Israel',
			'IM'=>'Isle of Man',
			'IN'=>'India',
			'IO'=>'British Indian Ocean Territory (the)',
			'IQ'=>'Iraq',
			'IR'=>'Iran (Islamic Republic of)',
			'IS'=>'Iceland',
			'IT'=>'Italy',
			'JE'=>'Jersey',
			'JM'=>'Jamaica',
			'JO'=>'Jordan',
			'JP'=>'Japan',
			'KE'=>'Kenya',
			'KG'=>'Kyrgyzstan',
			'KH'=>'Cambodia',
			'KI'=>'Kiribati',
			'KM'=>'Comoros (the)',
			'KN'=>'Saint Kitts and Nevis',
			'KP'=>'Korea (the Democratic People\'s Republic of)',
			'KR'=>'Korea (the Republic of)',
			'KW'=>'Kuwait',
			'KY'=>'Cayman Islands (the)',
			'KZ'=>'Kazakhstan',
			'LA'=>'Lao People\'s Democratic Republic (the)',
			'LB'=>'Lebanon',
			'LC'=>'Saint Lucia',
			'LI'=>'Liechtenstein',
			'LK'=>'Sri Lanka',
			'LR'=>'Liberia',
			'LS'=>'Lesotho',
			'LT'=>'Lithuania',
			'LU'=>'Luxembourg',
			'LV'=>'Latvia',
			'LY'=>'Libya',
			'MA'=>'Morocco',
			'MC'=>'Monaco',
			'MD'=>'Moldova (the Republic of)',
			'ME'=>'Montenegro',
			'MF'=>'Saint Martin (French part)',
			'MG'=>'Madagascar',
			'MH'=>'Marshall Islands (the)',
			'MK'=>'North Macedonia',
			'ML'=>'Mali',
			'MM'=>'Myanmar',
			'MN'=>'Mongolia',
			'MO'=>'Macao',
			'MP'=>'Northern Mariana Islands (the)',
			'MQ'=>'Martinique',
			'MR'=>'Mauritania',
			'MS'=>'Montserrat',
			'MT'=>'Malta',
			'MU'=>'Mauritius',
			'MV'=>'Maldives',
			'MW'=>'Malawi',
			'MX'=>'Mexico',
			'MY'=>'Malaysia',
			'MZ'=>'Mozambique',
			'NA'=>'Namibia',
			'NC'=>'New Caledonia',
			'NE'=>'Niger (the)',
			'NF'=>'Norfolk Island',
			'NG'=>'Nigeria',
			'NI'=>'Nicaragua',
			'NL'=>'Netherlands (the)',
			'NO'=>'Norway',
			'NP'=>'Nepal',
			'NR'=>'Nauru',
			'NU'=>'Niue',
			'NZ'=>'New Zealand',
			'OM'=>'Oman',
			'PA'=>'Panama',
			'PE'=>'Peru',
			'PF'=>'French Polynesia',
			'PG'=>'Papua New Guinea',
			'PH'=>'Philippines (the)',
			'PK'=>'Pakistan',
			'PL'=>'Poland',
			'PM'=>'Saint Pierre and Miquelon',
			'PN'=>'Pitcairn',
			'PR'=>'Puerto Rico',
			'PS'=>'Palestine, State of',
			'PT'=>'Portugal',
			'PW'=>'Palau',
			'PY'=>'Paraguay',
			'QA'=>'Qatar',
			'RE'=>'Réunion',
			'RO'=>'Romania',
			'RS'=>'Serbia',
			'RU'=>'Russian Federation (the)',
			'RW'=>'Rwanda',
			'SA'=>'Saudi Arabia',
			'SB'=>'Solomon Islands',
			'SC'=>'Seychelles',
			'SD'=>'Sudan (the)',
			'SE'=>'Sweden',
			'SG'=>'Singapore',
			'SH'=>'Saint Helena, Ascension and Tristan da Cunha',
			'SI'=>'Slovenia',
			'SJ'=>'Svalbard and Jan Mayen',
			'SK'=>'Slovakia',
			'SL'=>'Sierra Leone',
			'SM'=>'San Marino',
			'SN'=>'Senegal',
			'SO'=>'Somalia',
			'SR'=>'Suriname',
			'SS'=>'South Sudan',
			'ST'=>'Sao Tome and Principe',
			'SV'=>'El Salvador',
			'SX'=>'Sint Maarten (Dutch part)',
			'SY'=>'Syrian Arab Republic (the)',
			'SZ'=>'Eswatini',
			'TC'=>'Turks and Caicos Islands (the)',
			'TD'=>'Chad',
			'TF'=>'French Southern Territories (the)',
			'TG'=>'Togo',
			'TH'=>'Thailand',
			'TJ'=>'Tajikistan',
			'TK'=>'Tokelau',
			'TL'=>'Timor-Leste',
			'TM'=>'Turkmenistan',
			'TN'=>'Tunisia',
			'TO'=>'Tonga',
			'TR'=>'Türkiye',
			'TT'=>'Trinidad and Tobago',
			'TV'=>'Tuvalu',
			'TW'=>'Taiwan (Province of China)',
			'TZ'=>'Tanzania, the United Republic of',
			'UA'=>'Ukraine',
			'UG'=>'Uganda',
			'UM'=>'United States Minor Outlying Islands (the)',
			'US'=>'United States of America (the)',
			'UY'=>'Uruguay',
			'UZ'=>'Uzbekistan',
			'VA'=>'Holy See (the)',
			'VC'=>'Saint Vincent and the Grenadines',
			'VE'=>'Venezuela (Bolivarian Republic of)',
			'VG'=>'Virgin Islands (British)',
			'VI'=>'Virgin Islands (U.S.)',
			'VN'=>'Viet Nam',
			'VU'=>'Vanuatu',
			'WF'=>'Wallis and Futuna',
			'WS'=>'Samoa',
			'YE'=>'Yemen',
			'YT'=>'Mayotte',
			'ZA'=>'South Africa',
			'ZM'=>'Zambia',
			'ZW'=>'Zimbabwe',
			'1A'=>'Kosovo',
			'XI'=>'United Kingdom (Northern Ireland)'
		);

		if(!strlen($code))
			return $a;
		else if(isset($a[$code]))
			return $a[$code];
		else
			return '['.$code.' !]';
	}

	public function fxcSchemeIDVAT($code='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$code=trim((string)$code);

		$a=array(
			'VAT'=>'Value added tax',
			'VA'=>'Value added tax',
			'FC'=>'Fiscal number'
		);

		if(!strlen($code))
			return $a;
		else if(isset($a[$code]))
			return $a[$code];
		else
			return '['.$code.' !]';
	}

	public function fxcCurrencyID($code='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$code=trim((string)$code);
		if(strlen($code))
			$code=strtoupper(substr($code,0,3));

		$a=array(
			'AED'=>'UAE Dirham',
			'AFN'=>'Afghani',
			'ALL'=>'Lek',
			'AMD'=>'Armenian Dram',
			'ANG'=>'Netherlands Antillean Guilder',
			'AOA'=>'Kwanza',
			'ARS'=>'Argentine Peso',
			'AUD'=>'Australian Dollar',
			'AWG'=>'Aruban Florin',
			'AZN'=>'Azerbaijan Manat',
			'BAM'=>'Convertible Mark',
			'BBD'=>'Barbados Dollar',
			'BDT'=>'Taka',
			'BGN'=>'Bulgarian Lev',
			'BHD'=>'Bahraini Dinar',
			'BIF'=>'Burundi Franc',
			'BMD'=>'Bermudian Dollar',
			'BND'=>'Brunei Dollar',
			'BOB'=>'Boliviano',
			'BOV'=>'Mvdol',
			'BRL'=>'Brazilian Real',
			'BSD'=>'Bahamian Dollar',
			'BTN'=>'Ngultrum',
			'BWP'=>'Pula',
			'BYN'=>'Belarusian Ruble',
			'BZD'=>'Belize Dollar',
			'CAD'=>'Canadian Dollar',
			'CDF'=>'Congolese Franc',
			'CHE'=>'WIR Euro',
			'CHF'=>'Swiss Franc',
			'CHW'=>'WIR Franc',
			'CLF'=>'Unidad de Fomento',
			'CLP'=>'Chilean Peso',
			'CNY'=>'Yuan Renminbi',
			'COP'=>'Colombian Peso',
			'COU'=>'Unidad de Valor Real',
			'CRC'=>'Costa Rican Colon',
			'CUC'=>'Peso Convertible',
			'CUP'=>'Cuban Peso',
			'CVE'=>'Cabo Verde Escudo',
			'CZK'=>'Czech Koruna',
			'DJF'=>'Djibouti Franc',
			'DKK'=>'Danish Krone',
			'DOP'=>'Dominican Peso',
			'DZD'=>'Algerian Dinar',
			'EGP'=>'Egyptian Pound',
			'ERN'=>'Nakfa',
			'ETB'=>'Ethiopian Birr',
			'EUR'=>'Euro',
			'FJD'=>'Fiji Dollar',
			'FKP'=>'Falkland Islands Pound',
			'GBP'=>'Pound Sterling',
			'GEL'=>'Lari',
			'GHS'=>'Ghana Cedi',
			'GIP'=>'Gibraltar Pound',
			'GMD'=>'Dalasi',
			'GNF'=>'Guinean Franc',
			'GTQ'=>'Quetzal',
			'GYD'=>'Guyana Dollar',
			'HKD'=>'Hong Kong Dollar',
			'HNL'=>'Lempira',
			'HTG'=>'Gourde',
			'HUF'=>'Forint',
			'IDR'=>'Rupiah',
			'ILS'=>'New Israeli Sheqel',
			'INR'=>'Indian Rupee',
			'IQD'=>'Iraqi Dinar',
			'IRR'=>'Iranian Rial',
			'ISK'=>'Iceland Krona',
			'JMD'=>'Jamaican Dollar',
			'JOD'=>'Jordanian Dinar',
			'JPY'=>'Yen',
			'KES'=>'Kenyan Shilling',
			'KGS'=>'Som',
			'KHR'=>'Riel',
			'KMF'=>'Comorian Franc ',
			'KPW'=>'North Korean Won',
			'KRW'=>'Won',
			'KWD'=>'Kuwaiti Dinar',
			'KYD'=>'Cayman Islands Dollar',
			'KZT'=>'Tenge',
			'LAK'=>'Lao Kip',
			'LBP'=>'Lebanese Pound',
			'LKR'=>'Sri Lanka Rupee',
			'LRD'=>'Liberian Dollar',
			'LSL'=>'Loti',
			'LYD'=>'Libyan Dinar',
			'MAD'=>'Moroccan Dirham',
			'MDL'=>'Moldovan Leu',
			'MGA'=>'Malagasy Ariary',
			'MKD'=>'Denar',
			'MMK'=>'Kyat',
			'MNT'=>'Tugrik',
			'MOP'=>'Pataca',
			'MRU'=>'Ouguiya',
			'MUR'=>'Mauritius Rupee',
			'MVR'=>'Rufiyaa',
			'MWK'=>'Malawi Kwacha',
			'MXN'=>'Mexican Peso',
			'MXV'=>'Mexican Unidad de Inversion (UDI)',
			'MYR'=>'Malaysian Ringgit',
			'MZN'=>'Mozambique Metical',
			'NAD'=>'Namibia Dollar',
			'NGN'=>'Naira',
			'NIO'=>'Cordoba Oro',
			'NOK'=>'Norwegian Krone',
			'NPR'=>'Nepalese Rupee',
			'NZD'=>'New Zealand Dollar',
			'OMR'=>'Rial Omani',
			'PAB'=>'Balboa',
			'PEN'=>'Sol',
			'PGK'=>'Kina',
			'PHP'=>'Philippine Peso',
			'PKR'=>'Pakistan Rupee',
			'PLN'=>'Zloty',
			'PYG'=>'Guarani',
			'QAR'=>'Qatari Rial',
			'RON'=>'Romanian Leu',
			'RSD'=>'Serbian Dinar',
			'RUB'=>'Russian Ruble',
			'RWF'=>'Rwanda Franc',
			'SAR'=>'Saudi Riyal',
			'SBD'=>'Solomon Islands Dollar',
			'SCR'=>'Seychelles Rupee',
			'SDG'=>'Sudanese Pound',
			'SEK'=>'Swedish Krona',
			'SGD'=>'Singapore Dollar',
			'SHP'=>'Saint Helena Pound',
			'SLE'=>'Sierra Leone (new valuation 2022)',
			'SOS'=>'Somali Shilling',
			'SRD'=>'Surinam Dollar',
			'SSP'=>'South Sudanese Pound',
			'STN'=>'Dobra',
			'SVC'=>'El Salvador Colon',
			'SYP'=>'Syrian Pound',
			'SZL'=>'Lilangeni',
			'THB'=>'Baht',
			'TJS'=>'Somoni',
			'TMT'=>'Turkmenistan New Manat',
			'TND'=>'Tunisian Dinar',
			'TOP'=>'Pa’anga',
			'TRY'=>'Turkish Lira',
			'TTD'=>'Trinidad and Tobago Dollar',
			'TWD'=>'New Taiwan Dollar',
			'TZS'=>'Tanzanian Shilling',
			'UAH'=>'Hryvnia',
			'UGX'=>'Uganda Shilling',
			'USD'=>'US Dollar',
			'USN'=>'US Dollar (Next day)',
			'UYI'=>'Uruguay Peso en Unidades Indexadas (UI)',
			'UYU'=>'Peso Uruguayo',
			'UYW'=>'Unidad Previsional',
			'UZS'=>'Uzbekistan Sum',
			'VED'=>'Bolívar Soberano, new valuation',
			'VES'=>'Bolívar Soberano',
			'VND'=>'Dong',
			'VUV'=>'Vatu',
			'WST'=>'Tala',
			'XAF'=>'CFA Franc BEAC',
			'XAG'=>'Silver',
			'XAU'=>'Gold',
			'XBA'=>'Bond Markets Unit European Composite Unit (EURCO)',
			'XBB'=>'Bond Markets Unit European Monetary Unit (E.M.U.-6)',
			'XBC'=>'Bond Markets Unit European Unit of Account 9 (E.U.A.-9)',
			'XBD'=>'Bond Markets Unit European Unit of Account 17 (E.U.A.-17)',
			'XCD'=>'East Caribbean Dollar',
			'XDR'=>'SDR (Special Drawing Right)',
			'XOF'=>'CFA Franc BCEAO',
			'XPD'=>'Palladium',
			'XPF'=>'CFP Franc',
			'XPT'=>'Platinum',
			'XSU'=>'Sucre',
			'XTS'=>'Codes specifically reserved for testing purposes',
			'XUA'=>'ADB Unit of Account',
			'XXX'=>'The codes assigned for transactions where no currency is involved',
			'YER'=>'Yemeni Rial',
			'ZAR'=>'Rand',
			'ZMW'=>'Zambian Kwacha',
			'ZWG'=>'Zimbabwe Gold',
			'ZWL'=>'Zimbabwe Dollar'
		);

		if(!strlen($code))
			return $a;
		else if(isset($a[$code]))
			return $a[$code];
		else
			return '['.$code.' !]';
	}

	public function fxcPaymentType($code=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if(($code === 'zzz') || ($code === 'zzz'))
			return 'Mutually defined';

		$code=(int)$code;

		$a=array(
			1=>'Instrument not defined',
			2=>'Automated clearing house credit',
			3=>'Automated clearing house debit',
			4=>'ACH demand debit reversal',
			5=>'ACH demand credit reversal',
			6=>'ACH demand credit',
			7=>'ACH demand debit',
			8=>'Hold',
			9=>'National or regional clearing',
			10=>'In cash',
			11=>'ACH savings credit reversal',
			12=>'ACH savings debit reversal',
			13=>'ACH savings credit',
			14=>'ACH savings debit',
			15=>'Bookentry credit',
			16=>'Bookentry debit',
			17=>'ACH demand cash concentration/disbursement (CCD) credit',
			18=>'ACH demand cash concentration/disbursement (CCD) debit',
			19=>'ACH demand corporate trade payment (CTP) credit',
			20=>'Cheque',
			21=>'Banker\'s draft',
			22=>'Certified banker\'s draft',
			23=>'Bank cheque (issued by a banking or similar establishment)',
			24=>'Bill of exchange awaiting acceptance',
			25=>'Certified cheque',
			26=>'Local cheque',
			27=>'ACH demand corporate trade payment (CTP) debit',
			28=>'ACH demand corporate trade exchange (CTX) credit',
			29=>'ACH demand corporate trade exchange (CTX) debit',
			30=>'Credit transfer',
			31=>'Debit transfer',
			32=>'ACH demand cash concentration/disbursement plus (CCD+)',
			33=>'ACH demand cash concentration/disbursement plus (CCD+)',
			34=>'ACH prearranged payment and deposit (PPD)',
			35=>'ACH savings cash concentration/disbursement (CCD) credit',
			36=>'ACH savings cash concentration/disbursement (CCD) debit',
			37=>'ACH savings corporate trade payment (CTP) credit',
			38=>'ACH savings corporate trade payment (CTP) debit',
			39=>'ACH savings corporate trade exchange (CTX) credit',
			40=>'ACH savings corporate trade exchange (CTX) debit',
			41=>'ACH savings cash concentration/disbursement plus (CCD+)',
			42=>'Payment to bank account',
			43=>'ACH savings cash concentration/disbursement plus (CCD+)',
			44=>'Accepted bill of exchange',
			45=>'Referenced home-banking credit transfer',
			46=>'Interbank debit transfer',
			47=>'Home-banking debit transfer',
			48=>'Bank card',
			49=>'Direct debit',
			50=>'Payment by postgiro',
			51=>'FR, norme 6 97-Telereglement CFONB (French Organisation for',
			52=>'Urgent commercial payment',
			53=>'Urgent Treasury Payment',
			54=>'Credit card',
			55=>'Debit card',
			56=>'Bankgiro',
			57=>'Standing agreement',
			58=>'SEPA credit transfer',
			59=>'SEPA direct debit',
			60=>'Promissory note',
			61=>'Promissory note signed by the debtor',
			62=>'Promissory note signed by the debtor and endorsed by a bank',
			63=>'Promissory note signed by the debtor and endorsed by a',
			64=>'Promissory note signed by a bank',
			65=>'Promissory note signed by a bank and endorsed by another',
			66=>'Promissory note signed by a third party',
			67=>'Promissory note signed by a third party and endorsed by a',
			68=>'Online payment service',
			69=>'Transfer Advice',
			70=>'Bill drawn by the creditor on the debtor',
			74=>'Bill drawn by the creditor on a bank',
			75=>'Bill drawn by the creditor, endorsed by another bank',
			76=>'Bill drawn by the creditor on a bank and endorsed by a',
			77=>'Bill drawn by the creditor on a third party',
			78=>'Bill drawn by creditor on third party, accepted and',
			91=>'Not transferable banker\'s draft',
			92=>'Not transferable local cheque',
			93=>'Reference giro',
			94=>'Urgent giro',
			95=>'Free format giro',
			96=>'Requested method for payment was not used',
			97=>'Clearing between partners',
			98=>'JP, Electronically Recorded Monetary Claims',
			'ZZZ'=>'Mutually defined'
		);

		if(!$code)
			return $a;
		else if(isset($a[$code]))
			return $a[$code];
		else
			return '['.$code.' !]';
	}

	protected function fxcInt($v=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$i=(int)$v;

		return $i;
	}

	protected function fxcText($v='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		// Replace XML special characters like ["], ['], [<], [>] or [&]
		$v=strtr(trim((string)$v), $this->_tra);

		return $v;
	}

	protected function fxcRound($num, $dec=2)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$code=trim((string)$code);

		$str=(string)$num;
		$dpp=strpos($str,'.');
		if($dpp !== false)
			return substr($str,0,$dpp+1).substr(substr($str,$dpp+1).'0000',0,$dec);
		return $str.'.'.substr('0000',0,$dec);
	}
}
?>