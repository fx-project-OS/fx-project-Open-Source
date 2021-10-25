<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : resource.inc                                                 //
// Version     : 21.2                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-07-27                                                   //
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
 * Class for Resource Planning
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */
class RESOURCE
{
	// -----------------------------------------------------------------
	// VARIABLES														
	// =================================================================

	// ...Protected
	protected $_version			= '1.0';	// Version number of this class

	protected $_res_array		= array();				// Resources:	Array container of all resources with person id and process aid as primary key
	protected $_pif_array		= array();				// PersonInfos:	Array container of all persons with person id as primary key
	protected $_eff_array		= array();				// Effort:		Array container of all planned efforts with project id as primary key
	protected $_pnr_array		= array();				// Number:		Array container of all projects with process number as primary key

	protected $_debugmode		= 0;					// Bit 0=1: Funktionsaufruf
	protected $_use_transaction	= false;				// Eigene Datenbanktransaktion verwenden?
	protected $_ignore_status	= false;				// Projektstatus ignorieren?
	protected $_check_changes	= true;					// Festlegen, ob geprüft werden soll, wieviele Zeitdatensätze verändert wurden?
	protected $_send_emails		= true;					// Festlegen, ob Emails an Ressourcen versendet werden sollen?
	protected $_show_messages	= true;					// Festlegen, ob Erfolgsnachrichten angezeigt werden sollen?

	// ...Public
	public $mid					= 0;					// Mandanten-ID
	public $err					= false;				// Fehlervariable


	// -----------------------------------------------------------------
	// METHODS															
	// =================================================================


	////////////////////////////////////////////////////////////////////////////////
	// METHODS
	////////////////////////////////////////////////////////////////////////////////

	/**
	 * ...Class constructor
	 *
	 * @param boolean $use_transaction - Optional parameter (default = false): ???PARAMETER???
	 * @param boolean $ignore_status   - Optional parameter (default = false): ???PARAMETER???
	 * @param boolean $check_changes   - Optional parameter (default = true):  ???PARAMETER???
	 * @param boolean $send_emails     - Optional parameter (default = true):  ???PARAMETER???
	 * @param boolean $show_messages   - Optional parameter (default = true):  ???PARAMETER???
	 */
	public function __construct($use_transaction=false, $ignore_status=false, $check_changes=true, $send_emails=true, $show_messages=true)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if($use_transaction)
			$this->_use_transaction = true;

		if($ignore_status)
			$this->_ignore_status = true;

		if(!$check_changes)
			$this->_check_changes = false;

		if(isset($GLOBALS['nosuccess_msg']) && $GLOBALS['nosuccess_msg'])
			$this->_show_messages = false;

		if(!$send_emails)
			$this->_send_emails = false;

		if(!$show_messages)
			$this->_show_messages = false;

		$this->mid = $GLOBALS['fxpglobals']['client'];
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
	 * ...debug
	 */
	protected function debugFunction()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if(!($this->_debugmode&1))
			return;

		$trace	= debug_backtrace();
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
				else if(!strlen($val))
					echo('<font color=#ffbbbb>~</font>');
				else if(is_string($val))
					echo('<font color=#bbbbcc>&quot;</font>'.$val.'<font color=#bbbbcc>&quot;</font>');
				else
					echo($val);
			}
		}
		echo('<font color=#bbbbcc>)</font>;<br />');
	}

	/**
	 * ...convert2Array
	 *
	 * @param various $arr - Mandatory parameter: ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	protected function convert2Array($arr)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$pa = array();
		if(!is_array($arr))
		{
			if(strlen($arr))
			{
				$tmp = explode(',', $arr);
				foreach($tmp as $pid)
				{
					$pid = (int)$pid;
					if($pid > 0)
						$pa[] = $pid;
				}
			}
		}
		else if(!isset($arr[0]))
		{
			foreach($arr as $pid => $dmy)
			{
				$pid = (int)$pid;
				if($pid > 0)
					$pa[] = $pid;
			}
		}
		else
			$pa = $arr;
//fxDebug($pa, '$pa');

		return $pa;
	}

	/**
	 * ...version
	 *
	 * @param boolean $display - Optional parameter (default = false): ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function version($display=false)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();
		if($display)
			echo('fxp class: <b>RESOURCE</b> -- version: '.$this->_version.'<br />');
		else
			return $this->_version;
	}

	/**
	 * ...setDebugMode
	 *
	 * @param integer $debugmode - Optional parameter (default = 0): ???PARAMETER???
	 */
	public function setDebugMode($debugmode=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->_debugmode = (int)$debugmode;
	}

	/**
	 * ...setUseTransaction
	 *
	 * @param boolean $use_transaction - Optional parameter (default = false): ???PARAMETER???
	 */
	public function setUseTransaction($use_transaction=false)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->_use_transaction = false;
		if($use_transaction)
			$this->_use_transaction = true;
	}

	/**
	 * ...setIgnoreStatus
	 *
	 * @param boolean $ignore_status - Optional parameter (default = false): ???PARAMETER???
	 */
	public function setIgnoreStatus($ignore_status=false)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->_ignore_status = false;
		if($ignore_status)
			$this->_ignore_status = true;
	}

	/**
	 * ...setCheckChanges
	 *
	 * @param boolean $check_changes - Optional parameter (default = false): ???PARAMETER???
	 */
	public function setCheckChanges($check_changes=false)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->_check_changes = false;
		if($check_changes)
			$this->_check_changes = true;
	}

	/**
	 * ...setShowMessages
	 *
	 * @param boolean $show_messages - Optional parameter (default = false): ???PARAMETER???
	 */
	public function setShowMessages($show_messages=false)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->_show_messages = false;
		if($show_messages)
			$this->_show_messages = true;
	}

	/**
	 * ...setSendEmails
	 *
	 * @param boolean $send_emails - Optional parameter (default = false): ???PARAMETER???
	 */
	public function setSendEmails($send_emails=false)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->_send_emails = false;
		if($send_emails)
			$this->_send_emails = true;
	}

	/**
	 * ...debugResArray
	 *
	 * @param string $crf - Optional parameter (default = ''): ???PARAMETER???
	 * @param string $crl - Optional parameter (default = ''): ???PARAMETER???
	 */
	public function debugResArray($crf='', $crl='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		if(!strlen($crf))
		{
			$cra=debug_backtrace(); $crf=$cra[0]['file']; $crl=$cra[0]['line'];
		}
		fxDebug($this->_res_array, '$this->_res_array ("'.$crf.'" ['.$crl.'])');
	}

	/**
	 * ...getTasks
	 *
	 * @param various $arr - Mandatory parameter: ???PARAMETER???
	 */
	public function getTasks($arr)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$pa = $this->convert2Array($arr);
		if(is_array($pa) && sizeof($pa))
		{
			foreach($pa as $pid)
			{
				$pid = (int)$pid;
				if(($pid >= 0) && !isset($this->_eff_array[$pid]))
				{
					// Get process number
					$sql = "SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$pid." AND mandanten_id=".$this->mid;
					$pnr = trim(db_value($sql));
//fxDebug($pnr, $sql);
					if(strlen($pnr))
						$pnr .= '%';

					// Get processes
					$sql  = "SELECT DISTINCT p.projekt_id, p.projektart, p.vorgangsnummer, p.name_projekt, p.projekt_status, p.aufwand_soll, p.maart_id, p.mafaehigkeit_id,";
					$sql .= " z.soll_beginn, z.soll_ende ";
					$sql .= "FROM projekte p, zeitdaten z ";
					$sql .= "WHERE p.vorgangsnummer LIKE '".$pnr."' AND p.mandanten_id=".$this->mid;
					if(!$this->_ignore_status)
						$sql .= " AND p.projekt_status IN (".FXP_PS_PLANNED.",".FXP_PS_ACTIVE.")";
					$sql .= " AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$this->mid;
					$tmp  = db_values($sql, '*');
//fxDebug($tmp, $sql);
					if(is_array($tmp) && sizeof($tmp))
					{
						foreach($tmp as $t)
						{
							$prj = (int)$t['projekt_id'];

							// Effort array
							$this->_eff_array[$prj] = array('vorgangsnummer'=>$t['vorgangsnummer'], 'aufwand_soll'=>(int)$t['aufwand_soll']);

							// Process number array
							$this->_pnr_array[$t['vorgangsnummer']] = array('projekt_id'=>$prj, 'projektart'=>(int)$t['projektart'], 'name_projekt'=>$t['name_projekt'], 'projekt_status'=>$t['projekt_status'], 'soll_beginn'=>$t['soll_beginn'], 'soll_ende'=>$t['soll_ende'], 'aufwand_soll'=>(int)$t['aufwand_soll'], 'maart_id'=>(int)$t['maart_id'], 'mafaehigkeit_id'=>(int)$t['mafaehigkeit_id']);
						}
					}
				}
			}
		}
//fxDebug($this->_eff_array, '_eff_array');
//fxDebug($this->_pnr_array, '_pnr_array');
	}

	/**
	 * ...getPersonInfo
	 *
	 * @param various $res - Mandatory parameter: ???PARAMETER???
	 */
	public function getPersonInfo($res)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$res = (int)$res;
		if(($res > 0) && !isset($this->_pif_array[$res]))
		{
			$sql  = "SELECT p.pname, p.vorname,";
			$sql .= " m.arbeitstage, m.wochenarbeitstd, m.maeintrittsdatum, m.maaustrittsdatum, m.sonntag, m.montag, m.dienstag, m.mittwoch, m.donnerstag, m.freitag, m.samstag,";
			$sql .= " a.artikel_id, a.ek, a.vk ";
			$sql .= "FROM personen p, madaten m, artikel a ";
			$sql .= "WHERE p.personen_id=".$res." AND p.mandanten_id=".$this->mid." AND m.personen_id=p.personen_id AND m.mandanten_id=".$this->mid;
			$sql .= " AND a.personen_id=".$res." AND a.mandanten_id=".$this->mid." AND (a.sachmittel_id IS NULL OR a.sachmittel_id=0) AND (a.archiv IS NULL OR a.archiv=0)";
			$tmp  = db_values($sql);
			if(is_array($tmp))
			{
				$vorname	= trim($tmp['vorname']);
				$eintritt	= substr($tmp['maeintrittsdatum'], 0, 8);
				$austritt	= substr($tmp['maaustrittsdatum'], 0, 8);
				if(!strlen($austritt))
					$austritt = '99999999';
				$stundenprotag = 0.00;
				if($tmp['arbeitstage'])
					$stundenprotag = (double)($tmp['wochenarbeitstd'] / $tmp['arbeitstage']);
				$arbeitstage = (int)$tmp['sonntag'].(int)$tmp['montag'].(int)$tmp['dienstag'].(int)$tmp['mittwoch'].(int)$tmp['donnerstag'].(int)$tmp['freitag'].(int)$tmp['samstag'];

				$this->_pif_array[$res] = array('fullname'=>trim($tmp['pname']), 'entry'=>$eintritt, 'exit'=>$austritt, 'hoursperday'=>$stundenprotag, 'workdays'=>$arbeitstage, 'aid'=>(int)$tmp['artikel_id'], 'ek'=>get_double($tmp['ek']), 'vk'=>get_double($tmp['vk']));
				if(strlen($vorname))
					$this->_pif_array[$res]['fullname'] .= ', '.$vorname;
//fxDebug($this->_pif_array[$res], '_pif_array['.$res.']');
			}
		}
	}

	/**
	 * ...getResources
	 *
	 * @param various $arr - Mandatory parameter: ???PARAMETER???
	 */
	public function getResources($arr)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->getTasks($arr);
		$pa = $this->convert2Array($arr);
		if(is_array($pa) && sizeof($pa))
		{
			foreach($pa as $pid)
			{
				$pid = (int)$pid;
				if(($pid >= 0) && isset($this->_eff_array[$pid]))
				{
					$pnr = $this->_eff_array[$pid]['vorgangsnummer'].'%';

					// Get resources
					$sql  = "SELECT DISTINCT m.maplanung_id, m.personen_id, m.projekt_id, m.maart_id, m.mafaehigkeit_id, m.soll_beginn, m.soll_ende, m.auslastung, m.aufwand_std, m.prozent, m.e_mail, m.zeitstempel,";
					$sql .= " a.artikelzuord_id, a.ek_projekt, a.vk_projekt ";
					$sql .= "FROM maplanung m, artikelzuordnung a, projekte p ";
					$sql .= "WHERE m.projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$pnr."' AND mandanten_id=".$this->mid;
					if(!$this->_ignore_status)
						$sql .= " AND projekt_status IN (".FXP_PS_PLANNED.",".FXP_PS_ACTIVE.")";
					$sql .= ") AND (m.personen_id IS NOT NULL AND m.personen_id>0) AND m.mandanten_id=".$this->mid;
					$sql .= " AND m.maplanung_id=a.maplanung_id AND a.artikeltyp=1094 AND a.mandanten_id=".$this->mid;
					$tmp  = db_values($sql, '*');
//fxDebug($tmp, $sql);
					if(fxIsArray($tmp))
					{
						foreach($tmp as $t)
						{
							$res = (int)$t['personen_id'];
							$prj = (int)$t['projekt_id'];
							$pnr = $this->_eff_array[$prj]['vorgangsnummer'];

							$sql="SELECT projekt_status FROM projekte_history WHERE projekt_id=".$prj." AND mandanten_id=".$this->mid." AND zeitstempel<='".$t['zeitstempel']."' ORDER BY autoid DESC";
							$pst=(int)db_value($sql);
//fxDebug($pst,$sql);

							$this->getPersonInfo($res);

							// Resource array
							if(!isset($this->_res_array[$res]) || !is_array($this->_res_array[$res]))
								$this->_res_array[$res] = array();
							if(!isset($this->_res_array[$res][$prj]) || !is_array($this->_res_array[$res][$prj]))
								$this->_res_array[$res][$prj] = array();
							$this->_res_array[$res][$prj]['maplanung_id']		= (int)$t['maplanung_id'];
							$this->_res_array[$res][$prj]['artikelzuord_id']	= (int)$t['artikelzuord_id'];
							$this->_res_array[$res][$prj]['maart_id']			= (int)$t['maart_id'];
							$this->_res_array[$res][$prj]['mafaehigkeit_id']	= (int)$t['mafaehigkeit_id'];
							$this->_res_array[$res][$prj]['zeitstempel']		= $t['zeitstempel'];
							$this->_res_array[$res][$prj]['soll_beginn']		= $t['soll_beginn'];
							$this->_res_array[$res][$prj]['soll_ende']			= $t['soll_ende'];
							$this->_res_array[$res][$prj]['auslastung']			= get_double($t['auslastung']);
							$this->_res_array[$res][$prj]['aufwand_std']		= (int)$t['aufwand_std'];
							$this->_res_array[$res][$prj]['e_mail']				= (int)$t['e_mail'];
							$this->_res_array[$res][$prj]['prozent']			= get_double($t['prozent']);
							$this->_res_array[$res][$prj]['ek_projekt']			= get_double($t['ek_projekt']);
							$this->_res_array[$res][$prj]['vk_projekt']			= get_double($t['vk_projekt']);
							$this->_res_array[$res][$prj]['projekt_status']		= $pst;

							if(!isset($this->_res_array[$res][$prj]['assign']))
								$this->_res_array[$res][$prj]['assign'] = 1;

							if(!isset($this->_res_array[$res][$prj]['n_maart_id']))
								$this->_res_array[$res][$prj]['n_maart_id'] = $this->_pnr_array[$pnr]['maart_id'];
							if(!isset($this->_res_array[$res][$prj]['n_mafaehigkeit_id']))
								$this->_res_array[$res][$prj]['n_mafaehigkeit_id'] = $this->_pnr_array[$pnr]['mafaehigkeit_id'];
							if(!isset($this->_res_array[$res][$prj]['n_projekt_status']))
								$this->_res_array[$res][$prj]['n_projekt_status'] = $this->_pnr_array[$pnr]['projekt_status'];
							if(!isset($this->_res_array[$res][$prj]['n_aufwand_std']))
								$this->_res_array[$res][$prj]['n_aufwand_std'] = $this->_res_array[$res][$prj]['aufwand_std'];
							if(!isset($this->_res_array[$res][$prj]['n_prozent']))
								$this->_res_array[$res][$prj]['n_prozent'] = $this->_res_array[$res][$prj]['prozent'];
							if(!isset($this->_res_array[$res][$prj]['n_ek_projekt']))
								$this->_res_array[$res][$prj]['n_ek_projekt'] = $this->_res_array[$res][$prj]['ek_projekt'];
							if(!isset($this->_res_array[$res][$prj]['n_vk_projekt']))
								$this->_res_array[$res][$prj]['n_vk_projekt'] = $this->_res_array[$res][$prj]['vk_projekt'];
						}
					}
				}
			}
		}
//fxDebug($this->_res_array, '_res_array');
	}

	/**
	 * ...addPOST
	 *
	 * @param various $arr - Mandatory parameter: ???PARAMETER???
	 */
	public function addPOST($arr)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$errv = false;
		$this->getResources($arr);

		if(is_array($_POST) && isset($_POST['AUFW']) && is_array($_POST['AUFW']))
		{
			$budgetart = strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetart'], 0, 1)); // [N]=Keine Budgetart, [I]=Nur int. Budgetart, [E]=Nur ext. Budgetart + [B]=Beide Budgetarten
			foreach($_POST['AUFW'] as $res => $new_percent)
			{
				// Alte Zuordnung
				$old_assign = 0;
				if(is_array($_POST['AZO']) && $_POST['AZO'][$res])
					$old_assign = 1;
				// Neue Zuordnung
				$new_assign = 0;
				if(is_array($_POST['OK']) && $_POST['OK'][$res])
					$new_assign = 1;
				// Neu Aufwandprozentangabe
				if(strlen($new_percent))
				{
					$new_percent = validate_entry($new_percent, $errv, 'dezimal');
					if(!$new_percent)
						$new_percent = 0.00;
					else
						$new_percent = round($new_percent, 4);
				}
				// EK + VK
				if(($budgetart == 'N') || ($budgetart == 'E'))
					$ek = 0.00;
				else
				{
					$ek = '';
					if(strlen(trim($_POST['EK'][$res])))
						$ek = validate_entry($_POST['EK'][$res], $errv, 'betrag');
				}
				if(($budgetart == 'N') || ($budgetart == 'I'))
					$vk = 0.00;
				else
				{
					$vk = '';
					if(strlen(trim($_POST['VK'][$res])))
						$vk = validate_entry($_POST['VK'][$res], $errv, 'betrag');
				}

				// Zuordnungshaken ist gesetzt
				if($new_assign)
					$this->addResource($arr, $res, $new_percent, $ek, $vk);
				// Zuordnungshaken ist nicht gesetzt
				else
				{
					// Zuordnungshaken ist nicht gesetzt + Zuordnungshaken war gesetzt
					if($old_assign)
						$this->delResource($arr, $res);
					// Zuordnungshaken ist nicht gesetzt + Zuordnungshaken war nicht gesetzt + Aufwandsprozentangabe ist festgelegt
					else if(strlen($new_percent))
						$this->addResource($arr, $res, $new_percent, $ek, $vk);
				}
			}
		}
	}

	/**
	 * ...addResource
	 *
	 * @param various $arr     - Mandatory parameter:                ???PARAMETER???
	 * @param various $res     - Mandatory parameter:                ???PARAMETER???
	 * @param string  $percent - Optional parameter (default = ''):  ???PARAMETER???
	 * @param string  $ek      - Optional parameter (default = ''):  ???PARAMETER???
	 * @param string  $vk      - Optional parameter (default = ''):  ???PARAMETER???
	 */
	public function addResource($arr, $res, $percent='', $ek='', $vk='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->getTasks($arr);
		$pa = $this->convert2Array($arr);
		if(is_array($pa) && sizeof($pa))
		{
			$res = (int)$res;

			$this->getPersonInfo($res);

			$pmode=true;

			$peffort = 0;
			$percent = trim($percent);
			if(strlen($percent))
			{
				if(substr($percent,0,1) == 's')
				{
					$pmode = false;
					$peffort = (int)substr($percent,1);
				}
				else
				{
					$percent = get_double($percent);
					if(!$percent)
						$percent = 0.00;
					else
						$percent = round($percent/100.00, 4);
				}
			}
			else
				$percent = 1.00;

			$ek = trim($ek);
			if(strlen($ek))
			{
				$ek = get_double($ek);
				if(!$ek)
					$ek = 0.00;
				else
					$ek = round($ek, 2);
			}

			$vk = trim($vk);
			if(strlen($vk))
			{
				$vk = get_double($vk);
				if(!$vk)
					$vk = 0.00;
				else
					$vk = round($vk, 2);
			}

			foreach($pa as $pid)
			{
				$pid = (int)$pid;
				if(($pid >= 0) && isset($this->_eff_array[$pid]) && isset($this->_pif_array[$res]))
				{
					$pnr = $this->_eff_array[$pid]['vorgangsnummer'];
					$pnl = strlen($pnr);
					if(isset($this->_pnr_array[$pnr]))
					{
						foreach($this->_pnr_array as $tpn => $tdata)
						{
							if((substr($tpn, 0, $pnl) == $pnr) && ($tdata['projektart'] == FXP_PRJ_TASK) && ($this->_ignore_status || ($tdata['projekt_status'] == FXP_PS_PLANNED) || ($tdata['projekt_status'] == FXP_PS_ACTIVE)))
							{
								$prj = $tdata['projekt_id'];
								// Resource array
								if(!isset($this->_res_array[$res]) || !is_array($this->_res_array[$res]))
									$this->_res_array[$res] = array();
								if(!isset($this->_res_array[$res][$prj]) || !is_array($this->_res_array[$res][$prj]))
									$this->_res_array[$res][$prj] = array();

								$this->_res_array[$res][$prj]['assign'] = 2;

								$this->_res_array[$res][$prj]['n_maart_id']			= $this->_pnr_array[$tpn]['maart_id'];
								$this->_res_array[$res][$prj]['n_mafaehigkeit_id']	= $this->_pnr_array[$tpn]['mafaehigkeit_id'];

								if(!$peffort)
								{
									$this->_res_array[$res][$prj]['n_aufwand_std']		= (int)($this->_pnr_array[$tpn]['aufwand_soll'] * $percent);
									$this->_res_array[$res][$prj]['n_prozent']			= $percent;
								}
								else
								{
									$this->_res_array[$res][$prj]['n_aufwand_std']		= $peffort;
									$this->_res_array[$res][$prj]['n_prozent']			= round($peffort/$this->_pnr_array[$tpn]['aufwand_soll'], 4);
								}

								if(strlen($ek))
									$this->_res_array[$res][$prj]['n_ek_projekt'] = $ek;
								else if(!isset($this->_res_array[$res][$prj]['n_ek_projekt']))
									$this->_res_array[$res][$prj]['n_ek_projekt'] = $this->_pif_array[$res]['ek'];

								if(strlen($vk))
									$this->_res_array[$res][$prj]['n_vk_projekt'] = $vk;
								else if(!isset($this->_res_array[$res][$prj]['n_vk_projekt']))
									$this->_res_array[$res][$prj]['n_vk_projekt'] = $this->_pif_array[$res]['vk'];
							}
						}
					}
				}
			}
		}
//fxDebug($this->_res_array[$res], '_res_array['.$res.']');
	}

	/**
	 * ...delResource
	 *
	 * @param various $arr - Mandatory parameter: ???PARAMETER???
	 * @param various $res - Mandatory parameter: ???PARAMETER???
	 */
	public function delResource($arr, $res)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$this->getTasks($arr);
		$pa = $this->convert2Array($arr);
		if(is_array($pa) && sizeof($pa))
		{
			$res = (int)$res;

			$this->getPersonInfo($res);

			foreach($pa as $pid)
			{
				$pid = (int)$pid;
				if(($pid >= 0) && isset($this->_eff_array[$pid]) && isset($this->_pif_array[$res]))
				{
					$pnr = $this->_eff_array[$pid]['vorgangsnummer'];
					$pnl = strlen($pnr);
					if(isset($this->_pnr_array[$pnr]))
					{
						foreach($this->_pnr_array as $tpn => $tdata)
						{
							if((substr($tpn, 0, $pnl) == $pnr) && ($tdata['projektart'] == FXP_PRJ_TASK) && ($this->_ignore_status || ($tdata['projekt_status'] == FXP_PS_PLANNED) || ($tdata['projekt_status'] == FXP_PS_ACTIVE)))
							{
								$prj = $tdata['projekt_id'];
								// Resource array
								if(isset($this->_res_array[$res]) && is_array($this->_res_array[$res]) && isset($this->_res_array[$res][$prj]) && is_array($this->_res_array[$res][$prj]))
									$this->_res_array[$res][$prj]['assign'] = 0;
							}
						}
					}
				}
			}
		}
//fxDebug($this->_res_array[$res], '_res_array['.$res.']');
	}

	/**
	 * ...delResources
	 *
	 * @param various $arr - Mandatory parameter: ???PARAMETER???
	 */
	public function delResources($arr)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$pa = $this->convert2Array($arr);
		if(is_array($pa) && sizeof($pa))
		{
			$this->getResources($arr);

			foreach($pa as $pid)
			{
				$pid = (int)$pid;
				if(($pid >= 0) && isset($this->_eff_array[$pid]) && sizeof($this->_res_array))
				{
					foreach($this->_res_array as $res => $rdata)
					{
						foreach($rdata as $prj => $pdata)
						{
							if($prj == $pid)
								$this->delResource($prj, $res);
						}
					}
				}
			}
		}
//fxDebug($this->_res_array[$res], '_res_array['.$res.']');
	}

	/**
	 * ...assignResources
	 *
	 * @return ???RETURN???
	 */
	public function assignResources()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		if(sizeof($this->_res_array))
		{
			foreach($this->_res_array as $res => $rdata)
			{
				foreach($rdata as $prj => $pdata)
				{
					// Alte Zuordnung
					$old_assign  = 0;
					$old_peffort = 0;
					$old_percent = '';
					if(isset($pdata['prozent']))
					{
						$old_assign  = 1;
						$old_peffort = $pdata['aufwand_std'];
						$old_percent = round($pdata['prozent']*100.00, 2);
					}
					// Neue Zuordnung
					$new_assign  = 0;
					if(isset($pdata['assign']) && $pdata['assign'])
						$new_assign = 1;
					$new_peffort = 0;
					if((isset($pdata['n_aufwand_std'])) && strlen($pdata['n_aufwand_std']))
						$new_peffort = $pdata['n_aufwand_std'];
					$new_percent = '';
					if((isset($pdata['n_prozent'])) && strlen($pdata['n_prozent']))
						$new_percent = round($pdata['n_prozent']*100.00, 2);

					if($this->_debugmode&2)
						echo('<b>'.$res.'</b>: $old_assign='.$old_assign.', $new_assign='.$new_assign.' - $old_peffort='.$old_peffort.', $new_peffort='.$new_peffort.' - $old_percent='.$old_percent.', $new_percent='.$new_percent.'<br />'.$nl);

					// In der Matrix sind bereits korrekte Werte vorhanden
					if($GLOBALS['fxpglobals']['tr'] == 189)
					{
						if($new_assign)
							$this->_res_array[$res][$prj]['action'] = $new_percent;
						else if($old_assign)
							$this->_res_array[$res][$prj]['action'] = -666.00;
					}
					// Zuordnungshaken ist gesetzt
					else if($new_assign)
					{
						// Zuordnungshaken ist gesetzt + Zuordnungshaken war gesetzt
						if($old_assign)
						{
							// Zuordnungshaken ist gesetzt + Zuordnungshaken war gesetzt + Aufwandsprozentangabe ist festgelegt
							if(strlen($new_percent))
							{
								// Zuordnungshaken ist gesetzt + Zuordnungshaken war gesetzt + Aufwandsprozentangabe ist festgelegt + Aufwandsprozentangabe war festgelegt
								if(strlen($old_percent))
								{
									// Aufwandsprozentangaben blieben gleich
									if($new_percent == $old_percent)
										$this->_res_array[$res][$prj]['action'] = -$old_percent;
									// Aufwandsprozentangaben haben sich geändert
									else
										$this->_res_array[$res][$prj]['action'] = $new_percent;
								}
								// Zuordnungshaken ist gesetzt + Zuordnungshaken war gesetzt + Aufwandsprozentangabe ist festgelegt + Aufwandsprozentangabe war nicht festgelegt
								else
									$this->_res_array[$res][$prj]['action'] = $new_percent;
							}
							// Zuordnungshaken ist gesetzt + Zuordnungshaken war gesetzt + Aufwandsprozentangabe ist nicht festgelegt
							else
								$this->_res_array[$res][$prj]['action'] = -100.00;
						}
						// Zuordnungshaken war nicht gesetzt
						else
						{
							// Zuordnungshaken ist gesetzt + Zuordnungshaken war nicht gesetzt + Aufwandsprozentangabe ist festgelegt
							if(strlen($new_percent))
							{
								// Zuordnungshaken ist gesetzt + Zuordnungshaken war nicht gesetzt + Aufwandsprozentangabe ist festgelegt + Aufwandsprozentangabe war festgelegt
								if(strlen($old_percent))
								{
									// Aufwandsprozentangaben blieben gleich
									if($new_percent == $old_percent)
										$this->_res_array[$res][$prj]['action'] = -$old_percent;
									// Aufwandsprozentangaben haben sich geändert
									else
										$this->_res_array[$res][$prj]['action'] = $new_percent;
								}
								// Zuordnungshaken ist gesetzt + Zuordnungshaken war nicht gesetzt + Aufwandsprozentangabe ist festgelegt + Aufwandsprozentangabe war nicht festgelegt
								else
									$this->_res_array[$res][$prj]['action'] = $new_percent;
							}
							// Zuordnungshaken ist gesetzt + Zuordnungshaken war nicht gesetzt + Aufwandsprozentangabe ist nicht festgelegt
							else
								$this->_res_array[$res][$prj]['action'] = -333.00;
						} 
					}
					// Zuordnungshaken ist nicht gesetzt
					else
					{
						// Zuordnungshaken ist nicht gesetzt + Zuordnungshaken war gesetzt
						if($old_assign)
							$this->_res_array[$res][$prj]['action'] = -666.00;
						// Zuordnungshaken ist nicht gesetzt + Zuordnungshaken war nicht gesetzt
						else
						{
							// Zuordnungshaken ist nicht gesetzt + Zuordnungshaken war nicht gesetzt + Aufwandsprozentangabe ist festgelegt
							if(strlen($new_percent))
								$this->_res_array[$res][$prj]['action'] = $new_percent;
						}
					}
				}
			}

			$prj_done = array();
			foreach($this->_res_array as $res => $rdata)
			{
				foreach($rdata as $prj => $pdata)
				{
					if(!isset($prj_done[$prj]))
					{
						$prj_done[$prj] = true;

						if($GLOBALS['fxpglobals']['tr'] != 189)	// In der Matrix sind bereits korrekte Werte vorhanden
						{
							$calculation_run	= 1;

							$percent_total		= 0.00;	// Gesamtprozente
							$percent_fix		= 0.00;	// fest zugeteilte Prozente
							$percent_prorate	= 0.00;	// anteilsmäßige Prozente

							$persons_total		= 0;	// Gesamtanzahl zugeordneter Ressourcen
							$persons_delete		= 0;	// Anzahl zu löschender Ressourcen
							$persons_apportion	= 0;	// Anzahl gleichmäßig zuzuordnender Ressourcen
							$persons_prorate	= 0;	// Anzahl anteilsmäßig zuzuordnender Ressourcen

							$person_last_new	= 0;	// ID der letzten zugeordneten Ressourcen mit kalkulierten Prozenten
							$person_last_fix	= 0;	// ID der letzten zugeordneten Ressourcen mit festen Prozenten

							while($calculation_run < 5)
							{
								$percent_calculate = 100.00;
								if($calculation_run == 2)
									$percent_calculate = abs(100.00-$percent_fix);
								else if($calculation_run == 3)
									$percent_calculate = abs(100.00-$percent_total);
//echo('<b>BEFORE ['.$calculation_run.']</b>:<br />$persons_total='.$persons_total.', $persons_delete='.$persons_delete.', $persons_apportion='.$persons_apportion.', $persons_prorate='.$persons_prorate.'<br />$percent_total='.$percent_total.', $percent_fix='.$percent_fix.', $percent_calculate='.$percent_calculate.', $percent_prorate='.$percent_prorate.'<br />$person_last_new='.$person_last_new.', $person_last_fix='.$person_last_fix.'<hr>'.$nl);

								foreach($this->_res_array as $rres => $rrdata)
								{
									foreach($rrdata as $rprj => $rpdata)
									{
										if(($rprj == $prj) && isset($rpdata['action']))
										{
											$percent_value = $rpdata['action'];
//echo('$percent_value='.$percent_value.'<br />');

											switch($calculation_run)
											{
												case 1:	// Grundlagenermittlung
													if($percent_value > 0.00)
													{
														$percent_total	+= $percent_value;
														$percent_fix	+= $percent_value;
														$person_last_fix = $rres;
													}
													if($percent_value <= -666.00)
														$persons_delete++;
													else
													{
														$persons_total++;
														if($percent_value < -100.00)
														{
															$persons_apportion++;
															$person_last_new = $rres;
														}
														else if($percent_value < 0.00)
														{
															$persons_prorate++;
															$percent_prorate -= $percent_value;
															$person_last_new = $rres;
														}
													}
												break;

												case 2:	// Gleichmäßige Verteilung
													if(($percent_value > -666.00) && ($percent_value < -100.00))
													{
														$percent_new = round($percent_calculate / ($persons_apportion + $persons_prorate), 2);
														$this->_res_array[$rres][$rprj]['action'] = $percent_new;
														$percent_total	+= $percent_new;
													}
												break;

												case 3:	// Anteilsmäßige Verteilung
													if(($percent_value >= -100.00) && ($percent_value < 0.00))
													{
														$percent_new = round(($percent_calculate * -$percent_value) / $percent_prorate, 2);
														$this->_res_array[$rres][$rprj]['action'] = $percent_new;
														$percent_total	+= $percent_new;
													}
												break;

												case 4:	// Rest
													if($percent_total != 100.00)
													{
														if(($person_last_new && ($rres == $person_last_new)) || (!$person_last_new && $person_last_fix && ($rres == $person_last_fix)))
														{
															$percent_new = abs(100.00 - $percent_total + $percent_value);
															$this->_res_array[$rres][$rprj]['action'] = $percent_new;
															$percent_total = $percent_total - $percent_value + $percent_new;
															break 2;
														}
													}
													else
														break 2;
												break;
											}
										}
									}
								}
//echo('<b>AFTER ['.$calculation_run.']</b>:<br />$persons_total='.$persons_total.', $persons_delete='.$persons_delete.', $persons_apportion='.$persons_apportion.', $persons_prorate='.$persons_prorate.'<br />$percent_total='.$percent_total.', $percent_fix='.$percent_fix.', $percent_calculate='.$percent_calculate.', $percent_prorate='.$percent_prorate.'<br />$person_last_new='.$person_last_new.', $person_last_fix='.$person_last_fix.'<hr>'.$nl);

								if(($percent_total < 0.00) || ($percent_total > 101.00))
									break;

								$calculation_run++;
							}

							// 1590: Die Summe aller Aufwände muss exakt 100% ergeben!
							if(($percent_total < 0.00) || ($percent_total > 100.00))
							{
					   	   		$this->err = 1590;
					   	   		error_msg($this->err, false);
					   	   		return $this->err;
							}
						}
					}
				}
			}
			if($this->_debugmode&4)
				fxDebug($this->_res_array, '_res_array');

			// Ressourcen zuordnen oder entfernen
			if($this->_use_transaction)
				db_rollback('start'); // Transaktionsmarke setzen

			// Änderungen rücksetzen
			$delta_ekvk	= array();
			$ma_add		= 0;
			$ma_chg_m	= 0;
			$ma_chg_a	= 0;
			$ma_del		= 0;
			foreach($this->_res_array as $res => $rdata)
			{
				foreach($rdata as $prj => $pdata)
				{
					if(!isset($delta_ekvk[$prj]) || !is_array($delta_ekvk[$prj]))
						$delta_ekvk[$prj] = array('ek'=>0.00, 'vk'=>0.00);

					$percent_value = $pdata['action'];

					$pnr = $this->_eff_array[$prj]['vorgangsnummer'];
					$eff = $this->_eff_array[$prj]['aufwand_soll'];
					$typ = $this->_pnr_array[$pnr]['projektart'];
					$pls = $this->_pnr_array[$pnr]['soll_beginn'];
					$ple = $this->_pnr_array[$pnr]['soll_ende'];
//echo('$pnr='.$pnr.', $eff='.$eff.', $typ='.$typ.', $pls='.$pls.', $ple='.$ple.'<br />');

					// ...wenn der Aufwand >= 0.00 ist
					if(isset($pdata['action']) && ($percent_value >= 0.00))
					{
						if($GLOBALS['fxpglobals']['tr'] == 189)	// In der Matrix sind bereits korrekte Werte vorhanden
						{
							$percent=$pdata['n_prozent'];
							$effort_sec=$pdata['n_aufwand_std'];
						}
						else
						{
							$percent=$percent_value/100.00;
							$effort_sec=ceil($eff*$percent);
						}
						$utilization=auslastung($prj, $res, $effort_sec, max($this->_pif_array[$res]['entry'], substr($pls, 0, 8)), min($this->_pif_array[$res]['exit'], substr($ple, 0, 8)), $this->_pif_array[$res]['workdays'], $this->_pif_array[$res]['hoursperday']);

						if($this->_debugmode&2)
							echo($res.': $prj='.$prj.', $percent='.$percent.', $effort_sec='.$effort_sec.', $utilization='.$utilization.'<br />');

						$Benutzergr_ID	= '0';
						$sql_m			= "";
						$sql_a			= "";

						// Ressource bereits zugeordet, deshalb Zuordnungsdaten ändern
						if(isset($pdata['maplanung_id']))
						{
							if($this->_debugmode&2)
								echo('<b>Mitarbeiter '.$res.' zu Aufgabe '.$prj.' erneut hinzuordnen</b><br />');

							$E_Mail = '1';

							// Budget-Abgleich, wenn der EK/VK von bereits zugeteilten Personen geändert wird
							if((($pdata['ek_projekt'] != $pdata['n_ek_projekt']) || ($pdata['vk_projekt'] != $pdata['n_vk_projekt'])) && !$this->err)
							{
								// Updatet Zeiteinträge von der gewählten Person zu dem gewählten Projekt, die noch nicht abgerechnet oder vom GF freigegeben wurden
								$where  = " WHERE personen_id=".$res." AND projekt_id=".$prj." AND mandanten_id=".$this->mid." AND zeitaufwand>0";
								$where .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND (positions_id IS NULL OR positions_id=0) AND (freigabe IS NULL OR freigabe<3)";

								// Besteht überhaupt eine Zeiterfassung die noch geändert werden darf...
								$sql = "SELECT COUNT(zeit_id) AS summe FROM zeitdaten".$where;
								$changeable = (int)db_value($sql);
//fxDebug($changeable, '$changeable: '.$sql);
								// ... wenn nicht -> Hinweis OHNE Fehler
								if(!$changeable)
								{
									// 2104: Die EK- bzw. VK-Änderungen von $var1 wurden eingetragen, allerdings sind keine Zeitdatensätze zu der Aufgabe $var2 bearbeitet worden, da entweder keine vorhanden sind, alle bereits abgerechnet oder vom Geschäftsführer freigegeben wurden.
									$this->err = 2104;
									error_msg($this->err, false, '<b>'.$this->_pif_array[$res]['fullname'].'</b>', '&quot;['.no_null($pnr).'] '.$this->_pnr_array[$pnr]['name_projekt'].'&quot;');
								}
								// ...ansonsten Änderungen an Aufgabe vornehmen
								else if($typ == FXP_PRJ_TASK)
								{
									$cta = db_values("SELECT zeit_id, zeitaufwand FROM zeitdaten".$where, '*');
									if(is_array($cta))
									{
										foreach($cta as $ct)
										{
											if(!$this->err)
											{
												$cek = round($pdata['n_ek_projekt']*($ct['zeitaufwand']/3600), $GLOBALS['fxpglobals']['settings']['nkstellen']);
												$cvk = round($pdata['n_vk_projekt']*($ct['zeitaufwand']/3600), $GLOBALS['fxpglobals']['settings']['nkstellen']);
												$sql = "UPDATE zeitdaten SET kosten_ek=".$cek.", kosten_vk=".$cvk." WHERE zeit_id=".$ct['zeit_id']." AND mandanten_id=".$this->mid;
												$this->err = db_x($sql);
												if($this->_debugmode&4)
													fxDebug($this->err, $sql);
											}
										}
										if(!$this->err)
										{
											// Budget neu berechnen (Zeitaufwand*Lohn), auch bei jedem übergeordneten Projekt
											$this->err = Zeiterfassung($prid, 0, false, true);
											if($this->_debugmode&4)
												fxDebug($this->err, 'Korrektur des int. Budgets in der MaPlanung -> Zeiterfassung('.$prid.', 0, false, true);');
											if($this->err)
												error_msg($this->err, false);

											// Summary: Es wird geprüft, wieviele Datensätze nicht geändert wurden, da sie bereits abgerechnet wurden
											if(!$this->err && $this->_check_changes)
											{
												$sql = "SELECT COUNT(zeit_id) AS summe FROM zeitdaten WHERE personen_id=".$res." AND projekt_id=".$prj." AND zeitaufwand>0 AND (positions_id>0 OR freigabe=3) AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND mandanten_id=".$this->mid;
												$unchanged = (int)db_value($sql);
												if($this->_debugmode&4)
													fxDebug($unchanged, '$unchanged: '.$sql);
												if($unchanged)
												{
													$sql = "SELECT COUNT(zeit_id) AS summe FROM zeitdaten WHERE personen_id=".$res." AND projekt_id=".$prj." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND mandanten_id=".$this->mid;
													$total = (int)db_value($sql);
													if($this->_debugmode&4)
														fxDebug($total, '$total: '.$sql);
													// 2063: Von den vorhandenen $var1 Zeiteinträgen der Person $var2 zur Aufgabe $var3 wurden $var4 Einträge nicht aktualisiert, da diese bereits abgerechnet oder vom Geschäftsführer freigegeben wurden.
													error_msg(2063, false, $total, '<b>'.$this->_pif_array[$res]['fullname'].'</b>', '&quot;['.no_null($pnr).'] '.$this->_pnr_array[$pnr]['name_projekt'].'&quot;', $unchanged);
												}
											}
										}
									}
								}
							}

							$update_m = false;
							$update_a = false;
							if($pdata['maart_id'] != $pdata['n_maart_id'])
								$update_m = true;
							if($pdata['mafaehigkeit_id'] != $pdata['n_mafaehigkeit_id'])
								$update_m = true;
							if($pdata['auslastung'] != $utilization)
								$update_m = true;
							if($pdata['aufwand_std'] != $effort_sec)
								$update_m = true;
							if($pdata['prozent'] != $percent)
								$update_m = true;
							if($pdata['soll_beginn'] != $pls)
							{
								$update_m = true;
								$update_a = true;
							}
							if($pdata['soll_ende'] != $ple)
							{
								$update_m = true;
								$update_a = true;
							}
							if($pdata['ek_projekt'] != $pdata['n_ek_projekt'])
								$update_a = true;
							if($pdata['vk_projekt'] != $pdata['n_vk_projekt'])
								$update_a = true;
							if($pdata['projekt_status'] != $pdata['n_projekt_status'])
								$update_m = true;

							if($update_m)
							{
								$MaPlanung_ID = $pdata['maplanung_id'];

								$ma_chg_m++;

								$sql_m  = "UPDATE maplanung SET";
								$sql_m .= " maart_id=".$pdata['n_maart_id'].", mafaehigkeit_id=".$pdata['n_mafaehigkeit_id'].", soll_beginn='".$pls."', soll_ende='".$ple."',";
								$sql_m .= " auslastung=".$utilization.", aufwand_std=".$effort_sec.", prozent=".$percent." ";
								$sql_m .= "WHERE maplanung_id=".$MaPlanung_ID." AND mandanten_id=".$this->mid;
							}

							if($update_a)
							{
								$Artikelzuord_ID = $pdata['artikelzuord_id'];
								$ma_chg_a++;

								$sql_a  = "UPDATE artikelzuordnung SET";
								$sql_a .= " ek_projekt=".$pdata['n_ek_projekt'].", vk_projekt=".$pdata['n_vk_projekt'].", soll_beginn='".$pls."', soll_ende='".$ple."' ";
								$sql_a .= "WHERE artikelzuord_id=".$Artikelzuord_ID." AND mandanten_id=".$this->mid;
							}
						}

						// Ressource noch nicht zugeordet, deshalb neu zuordnen
						else
						{
							if($this->_debugmode&2)
								echo('<b>Mitarbeiter '.$res.' zu Aufgabe '.$prj.' neu hinzuordnen</b><br />');

							$E_Mail = '0';
							$ma_add++;

							$MaPlanung_ID = id_max('maplanung', 'maplanung_id');

							$sql_m  = "INSERT INTO maplanung";
							$sql_m .= " (mandanten_id, maplanung_id, personen_id, sachmittel_id, projekt_id, maart_id, mafaehigkeit_id, soll_beginn, soll_ende, auslastung, aufwand_std, prozent, e_mail, benutzergr_id)";
							$sql_m .= " VALUES";
							$sql_m .= " (".$this->mid.", ".$MaPlanung_ID.", ".$res.", NULL, ".$prj.", ".$pdata['n_maart_id'].", ".$pdata['n_mafaehigkeit_id'].", '".$pls."', '".$ple."', ".$utilization.", ".$effort_sec.", ".$percent.", ".$E_Mail.", ".$Benutzergr_ID.")";

							$Artikelzuord_ID = id_max('artikelzuordnung', 'artikelzuord_id');
							$Artikel_ID = (int)db_value("SELECT artikel_id FROM artikel WHERE personen_id=".$res." AND (sachmittel_id=0 OR sachmittel_id IS NULL) AND (archiv=0 OR archiv IS NULL) AND mandanten_id=".$this->mid);

							$sql_a  = "INSERT INTO artikelzuordnung (mandanten_id, artikelzuord_id, maplanung_id, artikel_id, artikeltyp, projekt_id, ek_projekt, vk_projekt, soll_beginn, soll_ende)";
							$sql_a .= " VALUES (".$this->mid.", ".$Artikelzuord_ID.", ".$MaPlanung_ID.", ".$this->_pif_array[$res]['aid'].", 1094, ".$prj.", ".$pdata['n_ek_projekt'].", ".$pdata['n_vk_projekt'].", '".$pls."', '".$ple."')";
						}

						$delta_ekvk[$prj]['ek'] += $percent*$pdata['n_ek_projekt'];
						$delta_ekvk[$prj]['vk'] += $percent*$pdata['n_vk_projekt'];

						if(!strlen($sql_m) && !strlen($sql_a))
							continue;

						if(strlen($sql_m))
						{
							if($this->_debugmode&4)
								echo('<b>-> MaPlanung:</b><br />'.$sql_m.'<br />'.$nl);
							$this->err = db_x($sql_m);
							if($this->err)
							{
				   	   			error_msg(1266, false, $this->_pif_array[$res]['fullname']);	// Mitarbeiter(in) "$var1" konnte dem Projekt nicht zugeteilt werden.
								break;
							}
						}

						if(strlen($sql_a))
						{
							if($this->_debugmode&4)
								echo('<b>-> Artikelzuordnung:</b><br />'.$sql_a.'<br />'.$nl);

							$this->err = db_x($sql_a);
							if($this->err)
							{
				   	   			error_msg(1267, false, $this->_pif_array[$res]['fullname']);	// Artikelinfo für Mitarbeiter(in) "$var1" konnte nicht erstellt werden.
								break;
							}
						}

						// Variable für Vorgänge (Matrix) zum Ermitteln der Projektänderungen
						if(isset($GLOBALS['pid_buffer']) && is_array($GLOBALS['pid_buffer']))
							$GLOBALS['pid_buffer'][$prj] = true;
					}

					// Ressource wurde aus Aufgabe entfernt
					else if(isset($pdata['action']) && ($percent_value == -666))
					{
						if($this->_debugmode&2)
							echo('<b>Mitarbeiter '.$res.' aus Aufgabe '.$prj.' entfernen</b><br />');

						$ma_del++;

						if(isset($GLOBALS['history_check_id']))
						{
							$hcib = $GLOBALS['history_check_id'];
							$GLOBALS['history_check_id'] = '';
							if(strlen($_POST['cp_4_'.$res]))
							{
								$hcib = '';
								$GLOBALS['history_check_id'] = $_POST['cp_4_'.$res];
							}
						}
						$sql_m = "DELETE FROM maplanung WHERE maplanung_id=".(int)$pdata['maplanung_id']." AND mandanten_id=".$this->mid;

						if($this->_debugmode&4)
							echo('<B>Alte Personen-Zuordnung löschen:</B><BR> -> '.$sql_m.'<BR>');

						$this->err = db_x($sql_m);
						if(isset($GLOBALS['history_check_id']))
							$GLOBALS['history_check_id'] = $hcib;

						if($this->err)
							break 2;

						$sql_a = "DELETE FROM artikelzuordnung WHERE artikelzuord_id=".(int)$pdata['artikelzuord_id']." AND mandanten_id=".$this->mid;

						if($this->_debugmode&4)
							echo('<B>Alte Artikel-Zuordnung löschen:</B><BR> -> '.$sql_a.'<BR>');

						$this->err = db_x($sql_a);
						if($this->err)
							break 2;

						// Variable für Vorgänge (Matrix) zum Ermitteln der Projektänderungen
						if(isset($GLOBALS['pid_buffer']) && is_array($GLOBALS['pid_buffer']))
							$GLOBALS['pid_buffer'][$prj] = true;
					}
				}
			}

			// E-Mails versenden
			$email_versendet=0;
			if(!$this->err && $this->_send_emails)
			{
				$msent=sendProjectMails();
				if($msent === false)	// Error
					error_msg(287,false);	// 287: Some emails could not be sent!
				else
					$email_versendet=$msent;
			}

			// Meldungen
			if(!$this->err && $this->_show_messages)
			{
				// Erfolgsmeldungen
				if($ma_add || $ma_chg_m || $ma_chg_a || $ma_del)
				{
					if($ma_add || $ma_del)
					{
						if($email_versendet)
							error_msg(1268, false);	// OK. Mitarbeiter(innen) wurden den Aufgaben zugeordnet bzw. entzogen und per Email benachrichtigt.
						else
							error_msg(1591, false);	// OK. Mitarbeiter(innen) wurden den Aufgaben zugeordnet bzw. entzogen.
					}
					else if($email_versendet)
						error_msg(2840, false);	// OK. Die zugeordneten Mitarbeiter(innen) der Aufgaben wurden erfolgreich angepasst und per Email benachrichtigt.
					else
						error_msg(2841, false);	// OK. Die zugeordneten Mitarbeiter(innen) der Aufgaben wurden erfolgreich angepasst.
				}
				else if($GLOBALS['fxpglobals']['tr'] == 43)
					error_msg(160, false);	// Es wurden keine Änderungen durchgeführt.
			}

			if($this->_use_transaction)
			{
				if($this->err) // Bei Fehler bisherige DB-Zugriffe wieder rückgängig machen
					db_rollback('abbruch');
				else // sonst Transaktion ausführen
					db_rollback('ok');
			}
		}

		return $this->err;
	}
}
?>