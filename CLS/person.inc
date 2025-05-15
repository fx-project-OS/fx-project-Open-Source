<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : person.inc                                                   //
// Version     : 24.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2024-03-14                                                   //
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
 * Class for Person Actions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */
class PERSON
{
	// -----------------------------------------------------------------
	// VARIABLES														
	// =================================================================

	// ...Protected
	protected $_version			= '1.0';		// Version number of this class

	protected $_pers_array		= array();				// Persons:	Array container of all persons with person id as primary key

	protected $_debugmode		= 0;					// Bit 0=1: Funktionsaufruf

	protected $_id_cnt			= -1;					// Id for new created persons, addresses or communications

	protected $_lookup			= array();				// Lookup values
	protected $_languages		= array();				// Language array
	protected $_ids				= array();				// Temp id

	protected $_pfields			= array(
		'*mandanten_id'			=> "i",		// *Primary key (Not null)
		'*personen_id'			=> "i",		// *Primary key (Not Null)
		'personentyp'			=> "i30",	// 274=Client, 275=Employee, 276=Customer, 620=Customer - Contact Person, 840=Contractor, 841=Contact - Contact Person, 1372=Contact, 2090=Client - Contact Person, 2091=Supplier/Company, 2092=Supplier - Contact Person
		'elter'					=> "i",
		'pname'					=> "s128",
		'vorname'				=> "s64",
		'personen_initialen'	=> "s40",
		'name_zusatz'			=> "s64",
		'anrede'				=> "i17",	// 310=Company, 311=Mr., 312=Ms., 2803=Mr./Ms.
		'perstitel'				=> "i25",
		'pposition'				=> "i23",
		'geschlecht'			=> "i56",	// 8=male, 9=female, 2802=unknown
		'abt_id'				=> "i",
		'vertragsstandort'		=> "i36",
		'einsatzort'			=> "i11",
		'inaktiv'				=> "c",
		'pzaehler'				=> "i",
		'gueltig_von'			=> "t",
		'gueltig_bis'			=> "t",
		'fr_id'					=> "s15"
	);
	protected $_afields			= array(
		'*mandanten_id'			=> "i",		// *Primary key (Not null)
		'*adressen_id'			=> "i",		// *Primary key (Not null)
		'#personen_id'			=> "i",		// #Not null
		'adresstyp'				=> "i95",	// 106=Main Address, 107=Additional Address
		'strasse'				=> "s128",
		'plz'					=> "s64",
		'ort'					=> "s64",
		'staat'					=> "s64",
		'landkz'				=> "i63",	// 847=D, 848=A, 849=CH, 850=F, 851=I, 854=GB, 866=USA
		'fr_id'					=> "s15"
	);
	protected $_cfields			= array(
		'*mandanten_id'			=> "i",		// *Primary key (Not null)
		'*kommunikation_id'		=> "i",		// *Primary key (Not null)
		'#personen_id'			=> "i",		// #Not null
		'kommunikationstyp'		=> "i94",	// 1768=VAT no. customer, 1872=Standard fax number, 1873=Standard mobile phone number, 2113=Invoice Address, 2121=Invoice Type, 2365=Standard phone number, 2366=Standard email, 2367=Standard homepage
		'kommunikation_txt'		=> "s255",
		'anmerkung'				=> "s",
		'fr_id'					=> "s15"
	);

	// ...Public
	public $cid					= 0;					// Client id
	public $cpid				= 0;					// Client person id
	public $lid					= 0;					// Language id
	public $sql_array			= array();				// SQL storage space
	public $err					= false;				// error variable


	// -----------------------------------------------------------------
	// METHODS															
	// =================================================================


	////////////////////////////////////////////////////////////////////////////////
	// METHODS
	////////////////////////////////////////////////////////////////////////////////

	/**
	 * ...Class constructor
	 */
	public function __construct()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		// Client id
		$this->cid = $GLOBALS['fxpglobals']['client'];

		// Client person id
		$this->cpid = (int)db_value("SELECT personen_id FROM personen WHERE personentyp=274 AND mandanten_id=".$this->cid);

		// Language id
		$this->lid = $GLOBALS['fxpglobals']['lang'];

		// Languages
		$la=fxGetAllLanguages();
		foreach($la as $lang => $a)
		{
			$act_lang=false;
			if($lang == $this->lid)
				$act_lang=true;

			$this->_languages[$lang] = $act_lang;
		}
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
			if(!is_null($arr) && strlen((string)$arr))
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
			echo('fxp class: <b>PERSON</b> -- version: '.$this->_version.'<br>');
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
	 * ...getLookup
	 *
	 * @param various $value    - Mandatory parameter:               ???PARAMETER???
	 * @param integer $field_id - Optional parameter (default = 0):  ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function getLookup($value, $field_id=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$field_id = (int)$field_id;
		if(is_null($value))
			$value = '';
		else
			$value = trim((string)$value);
		if(!strlen((string)$value))
			return 0;

		// Search id for this value
		$sql = "SELECT wertetabid FROM wertetab WHERE mandanten_id IN (0,".$this->cid.")";
		if($field_id > 0)
			$sql .= " AND id_feld=".$field_id;
		$sql .= " AND ".db_like('tabwert', $value);
		$id = (int)db_value($sql);

		// Id not found, then create it
		if(!$id)
			$id = $this->createLookup($value, $field_id);

		return $id;
	}

	/**
	 * ...createLookup
	 *
	 * @param various $value    - Mandatory parameter: ???PARAMETER???
	 * @param various $field_id - Mandatory parameter: ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function createLookup($value, $field_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$field_id = (int)$field_id;
		if(is_null($value))
			$value = '';
		else
			$value = trim((string)$value);
		if(!strlen((string)$value) || !fxIsArray($this->_languages))
			return 0;

		$id = id_max('wertetab', 'wertetabid', '', $this->cid);
		$value = convert_string($value, 'todb');
		// Create values in every language
		foreach($this->_languages as $lang => $act_lang)
		{
			$lvalue = $value;
			if(!$act_lang)
				$lvalue = '§'.$lvalue;

			$sql  = "INSERT INTO wertetab (mandanten_id, wertetabid, id_sprache, id_feld, position, tabwert, satzvers, archiv)";
			$sql .= " VALUES (".$this->cid.", ".$id.", ".$lang.", ".$field_id.", 0, '".$lvalue."', 0, 0)";
			db_x($sql);
		}

		return $id;
	}

	/**
	 * ...addPersonFields
	 *
	 * @param various $pid  - Mandatory parameter:                     ???PARAMETER???
	 * @param array   $data - Optional parameter (default = array()):  ???PARAMETER???
	 */
	public function addPersonFields($pid, $data=array())
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();
		$node = 'person';

		$pid = (int)$pid;
		if(!isset($this->_pers_array[$pid]))
			$this->_pers_array[$pid] = array();
		if(!isset($this->_pers_array[$pid][$node]))
			$this->_pers_array[$pid][$node] = array();

		foreach($this->_pfields as $pf => $tl)
		{
			$fcpf = substr((string)$pf, 0, 1);

			$pk		= false;
			$nn		= false;
			$field	= $pf;
			$type	= substr((string)$tl, 0, 1);
			$len	= (int)substr((string)$tl, 1);
			if(($fcpf == '*') || ($fcpf == '#'))	// *Primary key or #Not null
			{
				if($fcpf == '*')
					$pk = true;

				$nn = true;
				$field = substr((string)$pf, 1);
			}

			$def_value = '';
			if(is_array($data))
			{
				if(isset($data[$pf]))
					$def_value = $data[$pf];
				else if(isset($data[$field]))
					$def_value = $data[$field];

				if(strlen((string)$def_value) && ($type == "i") && $len)	// Lookup values
				{
					$def_int = (int)$def_value;
					if($def_int !== $def_value)
						$def_value = $this->getLookup($def_value, $len);
					else
						$def_value = $def_int;
				}
			}

			if(strlen((string)$def_value) || !isset($this->_pers_array[$pid][$node][$pf]))
			{
				if(strlen((string)$def_value))
				{
					if($type == "i")
						$this->_pers_array[$pid][$node][$pf] = (int)$def_value;
					else if($type == "d")
						$this->_pers_array[$pid][$node][$pf] = get_double($def_value);
					else
						$this->_pers_array[$pid][$node][$pf] = $def_value;
				}
				else if($field == 'mandanten_id')
					$this->_pers_array[$pid][$node][$pf] = $this->cid;
				else if($field == 'personen_id')
					$this->_pers_array[$pid][$node][$pf] = $pid;
				else if($field == 'elter')
					$this->_pers_array[$pid][$node][$pf] = $this->cpid;
				else if($nn && ($type == "i"))
					$this->_pers_array[$pid][$node][$pf] = 0;
				else if($nn && ($type == "d"))
					$this->_pers_array[$pid][$node][$pf] = 0.00;
				else
					$this->_pers_array[$pid][$node][$pf] = '';
			}
		}
	}

	/**
	 * ...addACFields
	 *
	 * @param various $mode - Mandatory parameter:                     ???PARAMETER???
	 * @param various $pid  - Mandatory parameter:                     ???PARAMETER???
	 * @param various $id   - Mandatory parameter:                     ???PARAMETER???
	 * @param array   $data - Optional parameter (default = array()):  ???PARAMETER???
	 */
	public function addACFields($mode, $pid, $id, $data=array())
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		if(strtolower(substr((string)$mode, 0, 1)) == 'a')
		{
			$node = 'addresses';
			$arr  = $this->_afields;
		}
		else
		{
			$node = 'communications';
			$arr  = $this->_cfields;
		}

		$pid = (int)$pid;
		if(!isset($this->_pers_array[$pid]))
			$this->_pers_array[$pid] = array();
		if(!isset($this->_pers_array[$pid][$node]))
			$this->_pers_array[$pid][$node] = array();

		$id = (int)$id;
		if(!isset($this->_pers_array[$pid][$node][$id]))
			$this->_pers_array[$pid][$node][$id] = array();

		foreach($arr as $pf => $tl)
		{
			$fcpf = substr((string)$pf, 0, 1);

			$pk		= false;
			$nn		= false;
			$field	= $pf;
			$type	= substr((string)$tl, 0, 1);
			$len	= (int)substr((string)$tl, 1);
			if(($fcpf == '*') || ($fcpf == '#'))	// *Primary key or #Not null
			{
				if($fcpf == '*')
					$pk = true;

				$nn = true;
				$field = substr((string)$pf, 1);
			}

			$def_value = '';
			if(is_array($data))
			{
				if(isset($data[$pf]))
					$def_value = $data[$pf];
				else if(isset($data[$field]))
					$def_value = $data[$field];

				if(strlen((string)$def_value) && ($type == "i") && $len)	// Lookup values
				{
					$def_int = (int)$def_value;
					if($def_int !== $def_value)
						$def_value = $this->getLookup($def_value, $len);
					else
						$def_value = $def_int;
				}
			}

			if(strlen((string)$def_value) || !isset($this->_pers_array[$pid][$node][$id][$pf]))
			{
				if(strlen((string)$def_value))
				{
					if($type == "i")
						$this->_pers_array[$pid][$node][$id][$pf] = (int)$def_value;
					else if($type == "d")
						$this->_pers_array[$pid][$node][$id][$pf] = get_double($def_value);
					else
						$this->_pers_array[$pid][$node][$id][$pf] = $def_value;
				}
				else if($field == 'mandanten_id')
					$this->_pers_array[$pid][$node][$id][$pf] = $this->cid;
				else if($field == 'personen_id')
					$this->_pers_array[$pid][$node][$id][$pf] = $pid;
				else if(($field == 'adressen_id') || ($field == 'kommunikation_id'))
					$this->_pers_array[$pid][$node][$id][$pf] = $id;
				else if($nn && ($type == "i"))
					$this->_pers_array[$pid][$node][$id][$pf] = 0;
				else if($nn && ($type == "d"))
					$this->_pers_array[$pid][$node][$id][$pf] = 0.00;
				else
					$this->_pers_array[$pid][$node][$id][$pf] = '';
			}
		}
	}

	/**
	 * ...getPerson
	 *
	 * @param various $pid - Mandatory parameter:                  ???PARAMETER???
	 * @param boolean $add - Optional parameter (default = true):  ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function getPerson($pid, $add=true)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$pid = (int)$pid;
		// New empty person array
		if(!$pid)
		{
			$pid = $this->_id_cnt;
			$this->_id_cnt--;
		}

		if($add)
			$this->addPersonFields($pid);

		return $pid;
	}

	/**
	 * ...getAddress
	 *
	 * @param various $pid - Mandatory parameter: ???PARAMETER???
	 * @param various $id  - Mandatory parameter: ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function getAddress($pid, $id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$pid = $this->getPerson($pid, false);

		// New empty address array
		$id = (int)$id;
		if(!$id)
		{
			$id = $this->_id_cnt;
			$this->_id_cnt--;
		}
		$this->addACFields('a', $pid, $id);

		return $id;
	}

	/**
	 * ...getCommunication
	 *
	 * @param various $pid - Mandatory parameter: ???PARAMETER???
	 * @param various $id  - Mandatory parameter: ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function getCommunication($pid, $id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		$pid = $this->getPerson($pid, false);

		// New empty communication array
		$id = (int)$id;
		if(!$id)
		{
			$id = $this->_id_cnt;
			$this->_id_cnt--;
		}
		$this->addACFields('c', $pid, $id);

		return $id;
	}

	/**
	 * ...addPerson
	 *
	 * @param integer $pid  - Optional parameter (default = 0):       ???PARAMETER???
	 * @param array   $data - Optional parameter (default = array()): ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function addPerson($pid=0, $data=array())
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		// Get existing person or create one
		$pid = $this->getPerson($pid, true);
		// Add data
		if(is_array($data) && sizeof($data))
			$this->addPersonFields($pid, $data);

		return $pid;
	}

	/**
	 * ...addAddress
	 *
	 * @param integer $pid  - Optional parameter (default = 0):       ???PARAMETER???
	 * @param integer $id   - Optional parameter (default = 0):       ???PARAMETER???
	 * @param array   $data - Optional parameter (default = array()): ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function addAddress($pid=0, $id=0, $data=array())
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		// Get existing address or create one
		$id = $this->getAddress($pid, $id);
		// Add data
		if(is_array($data) && sizeof($data))
			$this->addACFields('a', $pid, $id, $data);

		return $id;
	}

	/**
	 * ...addCommunication
	 *
	 * @param integer $pid  - Optional parameter (default = 0):       ???PARAMETER???
	 * @param integer $id   - Optional parameter (default = 0):       ???PARAMETER???
	 * @param array   $data - Optional parameter (default = array()): ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function addCommunication($pid=0, $id=0, $data=array())
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		// Get existing communication or create one
		$id = $this->getCommunication($pid, $id);
		// Add data
		if(is_array($data) && sizeof($data))
			$this->addACFields('c', $pid, $id, $data);

		return $id;
	}

	/**
	 * ...createSQLArray
	 *
	 * @return ???RETURN???
	 */
	public function createSQLArray()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debugFunction();

		if(fxIsArray($this->_pers_array))
		{
//fxDebug($this->_pers_array, '$this->_pers_array');

			$tables = array('person'=>"personen", 'addresses'=>"adressen", 'communications'=>"kommunikation");

			foreach($this->_pers_array as $pid => $pdata)
			{
				// Person id
				if($pid < 0)
				{
					if(!isset($this->_ids[$pid]))
						$this->_ids[$pid] = id_max('personen', 'personen_id');
//echo('$pid='.$pid.' -&gt; '.$this->_ids[$pid].'<br>');
				}

				foreach($tables as $tkey => $tname)
				{
					if(fxIsArray($pdata[$tkey]))
					{
						// Person
						if($tkey == 'person')
						{
							$sarray = array();
							foreach($pdata[$tkey] as $field => $value)
							{
								$fcpf = substr((string)$field, 0, 1);
								if(($fcpf == '*') || ($fcpf == '#'))	// *Primary key or #Not null
									$field = substr((string)$field, 1);

								if((($field == 'personen_id') || ($field == 'elter') || ($field == 'adressen_id') || ($field == 'kommunikation_id')) && ($value < 0))
									$value = $this->_ids[$value];

								if(!is_null($value) && strlen((string)$value))
								{
									if(is_string($value))
										$sarray[$field] = "'".convert_string($value)."'";
									else
										$sarray[$field] = $value;
								}
								else
									$sarray[$field] = "NULL";
							}
//fxDebug($sarray, '$sarray: '.$tname);

							// Insert oder Update
							$where = '';
							if($pid > 0)
								$where = "personen_id=".$pid;
							$sql = $this->createSQL($tname, $sarray, $where);
//fxDebug($sql, '$sql: '.$tname);
							if(strlen((string)$sql))
								$this->sql_array[] = $sql;
						}
						// Addresses + Communications
						else
						{
							foreach($pdata[$tkey] as $id => $acdata)
							{
								if($id < 0)
								{
									if(!isset($this->_ids[$id]))
									{
										if($tkey == 'addresses')
											$this->_ids[$id] = id_max('adressen', 'adressen_id');
										else
											$this->_ids[$id] = id_max('kommunikation', 'kommunikation_id');
//echo('$id='.$id.' -&gt; '.$this->_ids[$id].'<br>');
									}
								}

								$sarray = array();
								foreach($acdata as $field => $value)
								{
									$fcpf = substr((string)$field, 0, 1);
									if(($fcpf == '*') || ($fcpf == '#'))	// *Primary key or #Not null
										$field = substr((string)$field, 1);

									if((($field == 'personen_id') || ($field == 'elter') || ($field == 'adressen_id') || ($field == 'kommunikation_id')) && ($value < 0))
										$value = $this->_ids[$value];

									if(!is_null($value) && strlen((string)$value))
									{
										if(is_string($value))
											$sarray[$field] = "'".convert_string($value)."'";
										else
											$sarray[$field] = $value;
									}
									else
										$sarray[$field] = "NULL";
								}
//fxDebug($sarray, '$sarray: '.$tname);
								// Insert oder Update
								$where = '';
								if($id > 0)
								{
									if($tkey == 'addresses')
										$where = "adressen_id=".$id;
									else
										$where = "kommunikation_id=".$id;
								}
								$sql = $this->createSQL($tname, $sarray, $where);
//fxDebug($sql, '$sql: '.$tname);
							if(strlen((string)$sql))
								$this->sql_array[] = $sql;
							}
						}
					}
				}
			}
		}

//fxDebug($this->sql_array, '$this->sql_array');
		return $this->sql_array;
	}

	/**
	 * ???FUNCTION???
	 *
	 * @param boolean $arr - Optional parameter (default = false): ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function executeSQLArray($arr=false)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if($arr === false)
			$arr = $this->sql_array;
		if(fxIsArray($arr))
		{
			foreach($arr as $sql)
			{
//echo(fxHtmlEncode($sql).'<br>');
				$this->err = db_x($sql);
				if($this->err)
					break;
			}
		}

		return $this->err;
	}

	/**
	 * ???FUNCTION???
	 *
	 * @param various $tname  - Mandatory parameter:                ???PARAMETER???
	 * @param various $sarray - Mandatory parameter:                ???PARAMETER???
	 * @param string  $where  - Optional parameter (default = ""):  ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function createSQL($tname, $sarray, $where="")
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql = "";
		if(fxIsArray($sarray))
		{
			if(is_null($where) || !strlen((string)$where))
			{
				$sql = "INSERT INTO ".$tname." (";
				$scnt = 0;
				foreach($sarray as $skey => $svalue)
				{
					if($scnt)
						$sql .= ",";
					$sql .= $skey;

					$scnt++;
				}
				$sql .= ") VALUES (";
				$scnt = 0;
				foreach($sarray as $skey => $svalue)
				{
					if($scnt)
						$sql .= ",";
					$sql .= $svalue;

					$scnt++;
				}
				$sql .= ")";
			}
			// Update
			else
			{
				$sql = "UPDATE ".$tname." SET ";
				foreach($sarray as $skey => $svalue)
				{
					if($skey != 'mandanten_id')
						$sql .= $skey."=".$svalue;
				}
				$sql .= " WHERE ".$where." AND mandanten_id=".$sarray['mandanten_id'];
			}
		}

		return $sql;
	}
}
?>