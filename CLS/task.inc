<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : task.inc                                                     //
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
 * Class for Scheduler Tasks
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Get external mailer (PHPMailer) and mail function
$GLOBALS['__includer']['ext_mailer']=true;
$GLOBALS['__includer']['tools4']=true;
require('includer.inc');

$GLOBALS['class_task']=true;
$GLOBALS['class_fontstyle']='font-family:font-family:verdana,arial,helvetica,sans-serif;font-size:12pt;';

/**
 * @author Stefan Löwe
 * @TODO: Verbesserungen: Konzept Scheduler-Objekt das Programm-Objekten hat die Tasks besitzen,
 * da hier im Moment vorallem Programme und Tasks vermischt sind
 * 
 */
class TaskFactory
{
	////////////////////////////////////////////////////////////////////////////////
	// METHODS
	////////////////////////////////////////////////////////////////////////////////

	/**
	 * ???FUNCTION???
	 *
	 * @param integer $prog_id      - Optional parameter (default = 0): ???PARAMETER???
	 * @param integer $mandanten_id - Optional parameter (default = 0): ???PARAMETER???
	 *
	 * @return ???RETURN???
	 */
	public function __construct($prog_id=0, $mandanten_id=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if($prog_id && $mandanten_id)
			return $this->create_task($prog_id, $mandanten_id);
	}

	/**
	 * Klasse erstellt bei Übergabe einer Programm-ID und einer Mandanten-ID die passende Task
	 * @param int $prog_id die Programm-ID der Task die erstellt werden soll
	 * @param int $mandanten_id die ID des Mandanten für den die Task ausgeführt werden soll
	 * @return Objekt einer Unterklasse von Task
	 */
	function create_task($prog_id, $mandanten_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if($prog_id == FXP_SDL_TIMEREC)
			return new ZeitChkTask($prog_id, $mandanten_id);
		else if($prog_id == FXP_SDL_EMAIL)
			return new eMailTask($prog_id, $mandanten_id);
		else if($prog_id == FXP_SDL_WARNING)
			return new WarningTask($prog_id, $mandanten_id);
	}
}

/**
 * @author Stefan Löwe
 * Klasse zum Ausführen von Scheduler Tasks
 */
class Task
{
	var $prog_id		= null;
	var $task_id		= null;
	var $inactive		= false;
	var $intervall		= null;
	var $soll_beg_zeit	= null;
	var $soll_end_zeit	= null;
	var $next_start		= null;
	var $mandanten_id	= null;

	var $include_file	= '';
	var $err_task		= false;

	var $debug			= false;


	////////////////////////////////////////////////////////////////////////////////
	// METHODS
	////////////////////////////////////////////////////////////////////////////////

	/**
	 * Konstruktor der Klasse
	 * @param int $prog_id die Programm-ID der Task die erstellt werden soll
	 * @param int $mandanten_id die ID des Mandanten für den die Task ausgeführt werden soll
	 */
	public function __construct($prog_id, $mandanten_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$now=$GLOBALS['datetime'];
		$nowTime=getTimeString($now);
		$dateInfo=getdate(time());

		$sql  = "SELECT s.soll_beg_zeit, s.soll_end_zeit, s.intervall";
		$sql .= " FROM scheduler_settings s, wertetab w";
		$sql .= " WHERE s.prog_id=".$prog_id." AND s.mandanten_id=".$mandanten_id." AND s.".strtolower((string)getNameOfWeekday($dateInfo['wday']))."=1";
		$sql .= " AND ((s.soll_beg_zeit<>s.soll_end_zeit AND s.soll_beg_zeit<='".$nowTime."' AND s.soll_end_zeit>='".$nowTime."') OR (s.soll_beg_zeit=s.soll_end_zeit))";
		$sql .= " AND s.prog_id=w.wertetabid AND w.id_sprache=1 AND w.mandanten_id=0";

		$progData=db_values($sql);
//fxDebug($progData,$sql, 0);

		$this->inactive			= false;
		$this->prog_id			= $prog_id;
		$this->mandanten_id		= $mandanten_id;
		$this->intervall		= 0;
		$this->soll_beg_zeit	= '00000000000000';
		$this->soll_end_zeit	= '00000000240000';

		if(!is_array($progData))
			$this->inactive=true;
		else
		{
			$this->intervall=$progData['intervall'];
			$this->soll_beg_zeit=$progData['soll_beg_zeit'];
			$this->soll_end_zeit=$progData['soll_end_zeit'];
		}
	}

	/**
	 * ermittelt den nächsten Start der Task und legt diesen im Attribut next_start ab
	 * wird für den heutigen Tag keine Ausführung mehr geplant, so ist next_start - 1
	 */
	function determine_next_start()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$force=(int)$_GET['force'];

		// Programm inaktiv => nicht schedulen
		if($this->inactive && ($force != $this->prog_id))
		{
			$this->next_start=-1;
			$this->show('&middot; <i>Next start disabled</i> <i style="color:#888888;">(Program is inactive today)</i> = <b>'.$this->next_start.'</b><br />');
		}
		else
		{
			$now=$GLOBALS['datetime'];
			$startDate=substr((string)$now,0,8).substr((string)$this->soll_beg_zeit,8);
			$this->show('&middot; <i>Date:</i> <i style="color:#888888;">'.substr((string)$now,0,8).'</i> - <i>Start time:</i> <i style="color:#888888;">'.substr((string)$this->soll_beg_zeit,8).'</i>, <i>Now:</i> <i style="color:#888888;">'.substr((string)$now,8).'</i>, <i>End time:</i> <i style="color:#888888;">'.substr((string)$this->soll_end_zeit,8).'</i> - <i>Interval:</i> <i style="color:#888888;">'.$this->intervall.'</i><br />');
//echo('<br /><b>$now</b>='.$now.', <b>$this->soll_beg_zeit</b>='.$this->soll_beg_zeit.', <b>getTimeString($now)</b>='.getTimeString($now).', <b>$startDate</b>='.$startDate.'<br /><br />');

			if($force == $this->prog_id)
			{
				$this->next_start=zeitsek($startDate);
				$this->show('&middot; <i>Next start <b>FORCED</b></i> = <b>'.$this->next_start.'</b><br />');
			}
			// 1. Programmstart in der Zukunft => diesen "schedulen"
			else if(getTimeString($now) < $this->soll_beg_zeit)
			{
				$this->next_start=zeitsek($startDate);
				$this->show('&middot; <i>Next start via option <b style="color:#e1001a;">1</b></i> <i style="color:#888888;">(Start is in the future)</i> = <b>'.$this->next_start.'</b><br />');
			}
			// 2. Programmstart bereits in Vergangenheit aber Intervall gesetzt ...
			else if($this->intervall > 0)
			{
				// ... dann nächsten Start gemäß Intervall berechnen ...
				$faktor=ceil(time_diff($startDate,$now,'S') / $this->intervall);
				$startDate=time_add($startDate, $faktor * $this->intervall,'S');
				// ... ist dieser vor End-Zeit, dann übernehmen ...
				if(getTimeString($now) <= $this->soll_end_zeit)
				{
					$this->next_start=zeitsek($startDate);
					$this->show('&middot; <i>Next start via option <b style="color:#006b9f;">2.1</b></i> <i style="color:#888888;">(Start is in the past; With intervals; Before end time)</i> = <b>'.$this->next_start.'</b><br />');
				}
				// ... sonst nicht
				else
				{
					$this->next_start=-1;
					$this->show('&middot; <i>Next start via option <b style="color:#e1001a;">2.2</b></i> <i style="color:#888888;">(Start is in the past; With intervals; But after end time)</i> = <b>'.$this->next_start.'</b><br />');
				}
			}
			// 3. Programmstart bereits in Vergangenheit und kein Intervall gesetzt ...
			else
			{
				// ... dann zur nächsten vollen Minute schedulen falls heute noch nicht aktiv
				// und aktuelle Uhrzeit noch vor Endzeit oder Startzeit=Endzeit ...
				if((getDateString($this->get_prev_start()) < getDateString($now)) && ((getTimeString($now) <= $this->soll_end_zeit) || ($this->soll_beg_zeit == $this->soll_end_zeit)))
				{
					$this->next_start=zeitsek(substr((string)time_add($now,1,'N'),0,12).'00');
					$this->show('&middot; <i>Next start via option <b style="color:#006b9f;">3.1</b></i> <i style="color:#888888;">(Start is in the past; No intervals; Before end time)</i> = <b>'.$this->next_start.'</b><br />');
				}
				// ... sonst nicht
				else
				{
					$this->next_start=-1;
					$this->show('&middot; <i>Next start via option <b style="color:#e1001a;">3.2</b></i> <i style="color:#888888;">(Start is in the past; No intervals; But after end time)</i> = <b>'.$this->next_start.'</b><br />');
				}
			}
		}
	}

	/**
	 * ermittelt das Datum des letzen (abgeschlossenen) Starts der Task
	 * @return string das Datum des letzen (abgeschlossenen) Starts der Task
	 */
	function get_prev_start()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT max(ist_beginn) AS prev_start FROM scheduler_tasks";
		$sql .= " WHERE mandanten_id=".$this->mandanten_id." AND prog_id=".$this->prog_id." AND ist_ende IS NOT NULL";
		$value=db_value($sql);
//fxDebug($value,$sql, 0);

		return $value;
	}

	/**
	 * scheduled die Task, falls noch nicht geschehen, neu, setzt dabei implizit durch
	 * determine_next_start() next_start, und schreibt diese nächste Ausführungszeit
	 * in die Dantenbank
	 */
	function schedule()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->determine_next_start();
		if($this->next_start != -1)
		{
			$ntid=id_max('scheduler_tasks', 'task_id');

			$sql  = "INSERT INTO scheduler_tasks (mandanten_id, task_id, prog_id, soll_beginn)";
			$sql .= " VALUES (".$this->mandanten_id.", ".$ntid.", ".$this->prog_id.", '".date('YmdHis', $this->next_start)."')";
			$value=db_x($sql);
//fxDebug($value,$sql, 0);

			$err=$value;
		}
	}

	/**
	 * überprüft ob die Task bereits gescheduled ist, wenn ja wird next_start
	 * auf diesen Wert gesetzt
	 */
	function is_scheduled()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT soll_beginn FROM scheduler_tasks";
		$sql .= " WHERE mandanten_id=".$this->mandanten_id." AND prog_id=".$this->prog_id." AND ist_beginn IS NULL";
		$value=db_value($sql);
//fxDebug($value,$sql, 0);

		if($value == false)
			return false;
		else
		{
			$this->next_start=zeitsek($value);
			return true;
		}
	}

	/**
	 * führt die Task aus, falls bereits gescheduled oder neuer Scheduling-Versuch erfolgreich
	 * @return boolean $err true, wenn ein Datenbank-Fehler
	 * oder ein Fehler beim Ausführen der Task auftrat, false sonst
	 */
	function taskExecute()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$err=false;
		$force=(int)$_GET['force'];

		// Ausführung fortsetzen?
		if(!is_null($this->include_file) && strlen((string)$this->include_file))
		{
			$this->show('<b>Continue execution after include = &quot;'.$this->include_file.'&quot;</b><br />');

			$err=$this->update_ist_ende();
			if(!$err)
				$err=$this->schedule();

			$this->show('<hr style="border:0;border-top:1px solid #888888;" />');
			if($this->next_start != -1)
				$this->show('<b style="color:#009f6b;">New schedule set for: '.date('Ymd H:i:s',$this->next_start).'</b> <i>(in '.($this->next_start-time()).' sec)</i>');
			else
				$this->show('<b style="color:#e1001a;">No new schedule set</b>');
		}
		else
		{
			// kann sein dass hier inaktiv, obwohl gescheduled, z.B. wenn scheduler
			// nicht rechtzeitig erneut aufgerufen wird ... Problem?
			if($this->inactive && ($force != $this->prog_id))
			{
				$this->show('<b style="color:#e1001a;">Program is inactive today</b>');
				return;
			}
			else
			{
 				if($force == $this->prog_id)
	 				$this->show('<b style="color:#009f6b;">Activated (FORCED)</b><br /><br />');
 				else
	 				$this->show('<b style="color:#009f6b;">Activated</b><br /><br />');
				// Programm noch aktiv => abbrechen
				if($this->is_active())
				{
					$this->show('Program still running<br /><b>No new schedule set</b>');
					return;
				}
				else
					$this->show('Program not running at the moment');
			}

			if(!$this->is_scheduled())
			{
				$this->show(' and program not scheduled&hellip;<br />');
				$this->schedule();
				if($this->next_start == - 1)
					$this->show('<b>No new schedule set</b><br />');
				else
					$this->show('<b>New schedule set for: '.date('Ymd H:i:s',$this->next_start).'</b> <i>(in '.($this->next_start-time()).' sec)</i><br />');
			}
			else
				$this->show(' and program already scheduled for: <b>'.date('Ymd H:i:s',$this->next_start).'</b> <i>(in '.($this->next_start-time()).' sec)</i><br />');
			$this->show('<hr style="border:0;border-top:1px solid #888888;" />');

			if(($this->debug == $this->prog_id) || (($this->next_start != -1) && ($this->next_start < time())))
			{
				$this->show('<b style="color:#006b9f;">Start program</b>&hellip;<br />');
				$this->poll_task_id();
				if(!$err)
					$err=$this->report_activity();
				if(!$err)
					$err=$this->update_ist_beginn();
				$this->show('<hr style="border:0;border-top:1px solid #888888;" />');
				if(!$err)
				{
					$this->show('<b style="color:#006b9f;">Execute task</b>&hellip;<br />');

					// Task ausführen
					// ...Zeiterfassungs
					if($this->prog_id == FXP_SDL_TIMEREC)
					{
						$this->init_messages();
						$this->init_weekdays();
						$this->checkUserNotices();
						$this->checkSupervisorNotices();
					}
					// ...Email
					else if($this->prog_id == FXP_SDL_EMAIL)
					{
						$GLOBALS['_mail_mandantenid']=$this->mandanten_id;
						$this->err_task=$this->sendDMSMails();
					}
					// ...Warnungen
					else if($this->prog_id == FXP_SDL_WARNING)
					{
						$req_fn=fxf_fn_reqFilename('warning_functions', 143);
						if(!is_null($req_fn) && strlen((string)$req_fn))
							require($req_fn);
						$GLOBALS['_mail_mandantenid']=$this->mandanten_id;
						$this->err_task=executeWarningsScheduler($this->mandanten_id);
					}

					$this->show('<hr style="border:0;border-top:1px solid #888888;" />');
				}

				// Ausführung abbrechen falls Include-File ausgeführt werden soll
				if(is_null($this->include_file) || !strlen((string)$this->include_file))
				{
					if(!$err)
						$err=$this->update_ist_ende();
					if(!$err)
						$err=$this->schedule();

					$this->show('<hr style="border:0;border-top:1px solid #888888;" />');
					if($this->next_start != -1)
						$this->show('<b style="color:#009f6b;">New schedule set for: '.date('Ymd H:i:s',$this->next_start).'</b> <i>(in '.($this->next_start-time()).' sec)</i>');
					else
						$this->show('<b style="color:#e1001a;">No new schedule set</b>');
				}
				else
					$this->show('<b style="color:#e1001a;">Cancel execution because include = &quot;'.$this->include_file.'&quot;</b>');
			}
			else if($this->is_scheduled())
				$this->show('<b style="color:#009f6b;">Scheduled for: '.date('Ymd H:i:s',$this->next_start).'</b> <i>(in '.($this->next_start-time()).' sec)</i>');
			else
				$this->show('<b style="color:#e1001a;">Not scheduled!</b>');
		}

		if($err || $this->err_task)
			return true;
		else
			return false;
	}

	/**
	 * liefert die Task-ID
	 * @return int die Task-ID wenn diese Task gescheduled ist, false sonst
	 */
	function poll_task_id()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT task_id FROM scheduler_tasks";
		$sql .= " WHERE mandanten_id=".$this->mandanten_id." AND prog_id=".$this->prog_id." AND ist_beginn IS NULL";
		$value=db_value($sql);
//fxDebug($value,$sql, 0);

		$this->task_id=$value;
	}

	/**
	 * updated 'ist_beginn' einer Task
	 * @return boolean $err true, wenn ein Datenbank-Fehler auftrat, false sonst
	 */
	function update_ist_beginn()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "UPDATE scheduler_tasks SET ist_beginn='".$GLOBALS['datetime']."'";
		$sql .= " WHERE task_id=".$this->task_id." AND mandanten_id=".$this->mandanten_id;
		$value=db_x($sql);
//fxDebug($value,$sql, 0);

		return $value;
	}

	/**
	 * updated 'ist_ende', 'fehler' und 'was_running' einer Task
	 * @param int $task_id, die Task-ID falls Aufruf über $this->is_active()
	 * @return boolean $err true, wenn ein Datenbank-Fehler auftrat, false sonst 
	 */
	function update_ist_ende($task_id='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$tid=$this->task_id;
		if(!is_null($task_id) && (strlen((string)$task_id) > 0))
			$tid=$task_id;

		$fehler='NULL';
		if($this->err_task)
			$fehler=1;

		$sql  = "UPDATE scheduler_tasks SET ist_ende='".$GLOBALS['datetime']."', fehler=".$fehler.", was_running=1";
		$sql .= " WHERE task_id=".$tid." AND mandanten_id=".$this->mandanten_id;
		$value=db_x($sql);
//fxDebug($value,$sql, 0);

		return $value;
	}

	/**
	 * ermittelt, ob ein Task bei einem bestimmten Mandanten aktiviert ist, d.h. hier:
	 * 1. es muss als Sonderprogramm aktiviert sein
	 * 2. es muss mindestens einmal pro Woche laufen
	 * @param int $prog_id die Programm-ID
	 * @param int $mandanten_id die Mandaten-ID
	 * @return boolean true, wenn es noch obiger Definition aktiv ist, false sonst
	 * IST EIGENTLICH EINE STATISCHE FUNKTION, ABÄNDERN MIT PHP5
	 */
	function is_activated($prog_id, $mandanten_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT s.prog_id FROM scheduler_settings s";
		$sql .= " WHERE s.prog_id=".$prog_id." AND s.mandanten_id=".$mandanten_id." AND s.soll_beg_zeit<s.soll_end_zeit";
		$sql .= " AND (s.montag=1 OR s.dienstag=1 OR s.mittwoch=1 OR s.donnerstag=1 OR s.freitag=1 OR s.samstag=1 OR s.sonntag=1)";
		$value=db_value($sql);
//fxDebug($value,$sql, 0);

		if($value === false)
			return false;
		else
			return true;
	}

	/**
	 * ermittelt, ob ein Programm im Moment aktiv ist oder nicht
	 * @return boolean true, wenn Programm im Moment aktiv ist, false sonst
	 */
	function is_active()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT task_id, was_running FROM scheduler_tasks";
		$sql .= " WHERE prog_id=".$this->prog_id." AND mandanten_id=".$this->mandanten_id." AND ist_ende IS NULL ORDER BY soll_beginn DESC";
		$values=db_values($sql);
//fxDebug($values,$sql, 0);

		// 'was_running' = 1 => Programm bereits beendet => nicht aktiv
		if(is_null($values['task_id']) || !strlen((string)$values['task_id']) || is_null($values['was_running']) || !strlen((string)$values['was_running']) || ($values['was_running'] == 1))
			return false;
		// wenn mehr als 5 Minuten seit letzter Aktivität vergangen sind => nicht mehr aktiv ...
		else if(!is_null($values['task_id']) && strlen((string)$values['task_id']) && (time_diff($values['was_running'], $GLOBALS['datetime'], 'S') > 100))
		{
			// ... und Programm wird als beendet (abgestürzt = -1) markiert
			$this->update_ist_ende(-1, $values['task_id']);
			return false;
		}
		// ansonsten ist es noch aktiv
		else
			return true;
	}

	/**
	 * setzt das Feld "was_running" auf das aktuelle Datum oder für den Fall,
	 * dass $done gleich true ist auf 1
	 * @param boolean $done zu setzen wenn Programm als beendet markiert werden soll
	 * @return boolean true, wenn ein Fehler auftrat, false sonst
	 */
	function report_activity($done=false)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if($this->task_id && $this->mandanten_id)
		{
			$activity=$GLOBALS['datetime'];
			if($done)
				$activity=1;

			$this->show('&middot; <i>Report activity:</i> <i style="color:#888888;">'.$activity.'</i><br />');

			$sql  = "UPDATE scheduler_tasks SET was_running=".$activity;
			$sql .= " WHERE task_id=".$this->task_id." AND mandanten_id=".$this->mandanten_id;
			$value=db_x($sql);
//fxDebug($value,$sql, 0);

			return $value;
		}
		else
			return false;
	}

	/**
	 * ???FUNCTION???
	 *
	 * @param various $mode - Mandatory parameter: ???PARAMETER???
	 */
	function setDebugMode($mode)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->debug=(int)$mode;
	}

	/**
	 * ???FUNCTION???
	 *
	 * @param various $output - Mandatory parameter: ???PARAMETER???
	 */
	function show($output)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if($this->debug)
			echo $output;
	}
}

/**
 * @author Stefan Löwe
 * Klasse zum Ausführen des Zeiterfassungs-Checks
 */
class ZeitChkTask extends Task
{
	// entspricht dem Datum von gestern mit Uhrzeit gleich 00:00 Uhr
	var $date00 = null;

	// entspricht dem Datum von gestern mit Uhrzeit gleich 24:00 Uhr
	var $date24 = null;

	// Array das für alle Elter die Meldungen seiner zugeordneten Mitarbeiter enthält
	var $elter_warning=array();
	var $elter_lines=array();

	// Array das alle Textmeldungen enthält aus Tabelle "texte" mit negativem Index, aus "wertetab" mit positiven
	var $messages = null;

	// Array das alle Wochentagsnamenabkürzungen enthält
	var $weekdays = null;

	// ID's der Texte aus Tabelle "texte", mit negativen ID's um Kollisionen zu vermeiden
	var $TXT_NAME				= -14;	// Name
	var $TXT_DATUM				= -852;	// Datum
	var $TXT_AKTION				= -332;	// Aktion
	var $TXT_ZEITAUFWAND		= -274;	// Zeitaufwand

	// ID's der Texte aus Tabelle "wertetab"
	var $TXT_ZEIT_CHK			= 2166;	// Zeiterfassungscheck
	var $TXT_ELTER_MAIL_HEADER	= 2700;	// Ergebnis des Zeiterfassungschecks der zu "$var1" zugeordneten Mitarbeiter im Zeitraum vom $var2 bis zum $var3.

	var $TXT_ZEIT_IST_SOLL		= 1711;	// Zeitdaten (Ist/Soll)

	var $TXT_FRAU				= 312;	// Frau
	var $TXT_HERR				= 311;	// Herr

	var $TXT_STUNDEN			= 484;	// Std.
	var $TXT_MINUTEN			= 856;	// Min.

	var $TXT_EMP_NO_AUX_DATA	= 235;	// Dieser Mitarbeiter besitzt keine Zusatzdaten!
	var $TXT_EMP_NO_CHK			= 236;	// Bei diesem Mitarbeiter ist der Zeiterfassungscheck in den Zusatzdaten nicht aktiviert!
	var $TXT_EMP_NO_MAIL		= 1703;	// Diesem Mitarbeiter ist noch kein Benutzer mit einer gültigen E-Mail-Adresse zugeordnet!
	var $TXT_EMP_NO_WORKDAY		= 1704;	// Dieser Mitarbeiter muss an diesem Tag nicht arbeiten!
	var $TXT_EMP_APPOINTMENT	= 1705;	// Dieser Mitarbeiter ist an diesem Tag wegen eines Termins entschuldigt.
	var $TXT_AOK				= 1706;	// Alles ok! Zeiterfassung und Pause wurden vorschriftsmässig eingetragen.

	var $TXT_ERR_HOURS			= 1707;	// Es fehlen noch mindestens $var1 Stunde(n).
	var $TXT_ERR_TAKE_ACTION	= 1710;	// der automatische Zeiterfassungscheck für $var1 hat für Sie folgenden Hinweis zusammengestellt:


	////////////////////////////////////////////////////////////////////////////////
	// METHODS
	////////////////////////////////////////////////////////////////////////////////

	/**
	 * Konstruktor der Klasse
	 * @param int $prog_id die Programm-ID der Task die erstellt werden soll
	 * @param int $mandanten_id die ID des Mandanten für den die Task ausgeführt werden soll
	 */
	public function __construct($prog_id, $mandanten_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		parent::__construct($prog_id, $mandanten_id);
	}

	/**
	 * schickt an die Mitarbeiter des Mandanten eine Meldung, falls ihre Zeiterfassung
	 * nicht den Bestimmungen entspricht
	 */
	function checkUserNotices()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$yesterday=time()-86400;
		$this->date00=date('Ymd000000', $yesterday);
		$this->date24=date('Ymd240000', $yesterday);

		$wday=strtolower((string)getNameOfWeekday(date('w', $yesterday)));
		$this->show('<b>Check persons for:</b> '.$wday.', '.get_entry($this->date00,'date').'<br /><br />');

		$personal=$this->get_personal();
		if(fxIsArray($personal))
		{
			$GLOBALS['_mail_mandantenid']=$this->mandanten_id;

			$feiertag_faktor=$this->init_feiertag_faktor();
			$soll_prozent=$this->init_soll_prozent();

			$personen_ids=array_keys($personal);
			$fehl_termine=$this->get_fehl_termine($personen_ids);
			$ist_zeiten=$this->get_ist_zeiten($personen_ids);
			$email_addr=get_email_adress(implode(',', $personen_ids));

			foreach($personal as $pid => $person)
			{
				$name=$person['vorname'].' '.$person['pname'];
				setUserSettingsScheduler($pid, $this->mandanten_id);

				$this->show('<div style="border:1px dotted #888888;border-radius:6px;padding:4px;margin-bottom:2px;">');
				$error=$this->is_valid_person($person, $email_addr[$person['personen_id']], $wday);
				if($error == 0)
				{
					$this->show('<font style="color:#009f6b;">Check <b>'.$pid.'</b> <i>&quot;'.$name.'&quot;</i></font><br />');
					if(!isset($ist_zeiten[$pid]))
						$ist_zeiten[$pid]=array('personen_id'=>$pid, 'zeitaufwand'=>0, 'ist_beginn'=>"", 'ist_ende'=>"");

					$daily_workload=$person['wochenarbeitstd'] * 3600 / $person['arbeitstage'];
					$soll_sekunden=$daily_workload * $feiertag_faktor;
					$soll_sek_prozent=$soll_sekunden * $soll_prozent;
//					$this->show('$soll_sekunden: '.$soll_sekunden.', $soll_sek_prozent='.$soll_sek_prozent.'<br />');

					$fehlzeit=$this->get_fehlzeit($fehl_termine[$pid], $daily_workload);
					$ist_zeiten[$pid]['zeitaufwand'] += $fehlzeit;
//					$this->show('$ist_zeiten['.$pid.'][\'zeitaufwand\']: '.$ist_zeiten[$pid]['zeitaufwand'].'<br />');

					$fff=$feiertag_faktor + $fehlzeit;
//					$this->show('$fff ($feiertag_faktor='.$feiertag_faktor.' + $fehlzeit='.$fehlzeit.') = '.$fff.'<br />');

					$note='';
					if($fff)
					{
						// Ist-Zeitaufwand < Prozentanteil Soll-Zeitaufwand => Meldung dass Stunden fehlen
						$diff_prozent=$soll_sek_prozent - $ist_zeiten[$pid]['zeitaufwand'];
						if($diff_prozent > 0)
						{
							$note .= $this->get_task_message($this->TXT_ZEITAUFWAND).': ';															// Zeitaufwand:
							$note .= str_replace('$var1', get_entry($diff_prozent/3600, 'dezimal'), $this->get_task_message($this->TXT_ERR_HOURS));	// Es fehlen noch mindestens $var1 Stunde(n).
						}
					}
//					$this->show('$note: '.$note.'<br />');

					if(strlen((string)$note))
					{
						$weekday=fx_date('w', fx_mktime(12, 0, 0, (int)substr((string)$this->date00, 4, 2), (int)substr((string)$this->date00, 6, 2), (int)substr((string)$this->date00, 0, 4)));
						$datum=$this->get_weekday($weekday).', '.get_entry($this->date00, 'datum');
						$col=$this->get_color($this->TXT_ZEITAUFWAND);

						// Herr/Frau xyz,
						$mt  = '<font style="'.$GLOBALS['class_fontstyle'].'">'.$this->get_task_message($person['anrede']).' '.$name.',</font><br /><br />'.$GLOBALS['nl'];
						// der automatische Zeiterfassungscheck für $var1 hat für Sie folgenden Hinweis zusammengestellt:
						$mt .= '<font style="'.$GLOBALS['class_fontstyle'].'">'.str_replace('$var1', '<b style="'.$GLOBALS['class_fontstyle'].'">'.$datum.'</b>', $this->get_task_message($this->TXT_ERR_TAKE_ACTION)).'</font>'.$GLOBALS['nl'];
						// Mitteilung
						$mt .= '<br /><br /><br />'.$GLOBALS['nl'];
						$mt .= '<table width="80%" border="0" cellpadding="8" cellspacing="1" bgcolor="#cccccc" style="border-radius:4px;">'.$GLOBALS['nl'];
						$mt .= '	<tr>'.$GLOBALS['nl'];
						$mt .= '		<td nowrap bgcolor="#f8f8f8"><b style="color:#'.$col.';'.$GLOBALS['class_fontstyle'].'font-size:14pt;">'.$note.'</b></td>'.$GLOBALS['nl'];
						$mt .= '	</tr>'.$GLOBALS['nl'];
						$mt .= '</table>';

						// Zeiterfassungscheck
						$send_error=send_mail($pid, $this->get_task_message($this->TXT_ZEIT_CHK), $mt);
						if(!$send_error)
							$send_error='OK';
//fxDebug($send_error, '$send_error: '.$pid.' (strlen='.strlen((string)$mt).')', 0);

						if($this->debug == $this->prog_id)
							$this->show('<div style="padding:6px;background:#fff;border:1px solid #ccc;box-shadow:3px 3px 2px rgba(0,0,0, 0.25);">'.$mt.'</pre></div>');
						$this->show('&nbsp;&nbsp;&nbsp;&middot; <i>Send mail returned [<b>'.$send_error.'</b>]</i>');
					}
					else
						$this->show('&nbsp;&nbsp;&nbsp;&middot; <i>Everything OK - No notification needed</i>');
				}
				else
					$this->show('Don\'t check <b>'.$pid.'</b> <i>&quot;'.$name.'&quot;</i><br />&nbsp;&nbsp;&nbsp;&middot; <font style="color:#888888;">'.$this->get_task_message($error).'</font>');
				$this->show('</div>');
			}
		}
		$this->show('<hr style="border:0;border-top:1px solid #888888;" />');
	}

	/**
	 * schickt an die Vorgesetzen der jeweiligen Mitarbeiter eine Meldung,
	 * falls die Zeiterfassung der Mitarbeiter nicht den Bestimmungen entspricht
	 */
	function checkSupervisorNotices()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$elt_chk_active=$this->is_elter_chk_active();

		if($elt_chk_active > 0)
		{
			$start=date('Ymd000000', time()-86400*$elt_chk_active);
			$end=date('Ymd240000', time()-86400);

			$this->show('<b>Check supervisors for:</b> '.get_entry($start,'date').' - '.get_entry($end,'date').'<br /><br />');

			$personal=$this->get_personal($start,$end);
			if(fxIsArray($personal))
			{
				$GLOBALS['_mail_mandantenid']=$this->mandanten_id;

				$soll_prozent=$this->init_soll_prozent();
				$personen_ids=array_keys($personal);
				$email_addr=get_email_adress(implode(', ', $personen_ids));

				for($i=$elt_chk_active; $i>0; --$i)
				{
					$curr_date=time() - (86400 * $i);
					$wday=strtolower((string)getNameOfWeekday(date('w', $curr_date)));
					$this->date00=date('Ymd000000', $curr_date);
					$this->date24=date('Ymd240000', $curr_date);

					$feiertag_faktor=$this->init_feiertag_faktor();
					$ist_zeiten=$this->get_ist_zeiten($personen_ids);
					$fehl_termine=$this->get_fehl_termine($personen_ids);

					foreach($personal as $pid => $person)
					{
						setUserSettingsScheduler($person['elter'], $this->mandanten_id);
						$notice=array();

						// Person in diesem Zeitraum nicht angestellt => ... nächste Person
						if(($this->date00 < $person['maeintrittsdatum']) || (!is_null($person['maaustrittsdatum']) && strlen((string)$person['maaustrittsdatum']) && ($this->date00 >= $person['maaustrittsdatum'])))
							continue;

						if(!isset($ist_zeiten[$pid]))
							$ist_zeiten[$pid] = array('personen_id'=>$pid, 'zeitaufwand'=>0, 'ist_beginn'=>"", 'ist_ende'=>"");

						$daily_workload=$person['wochenarbeitstd'] * 3600 / $person['arbeitstage'];
						$soll_sekunden=$daily_workload * $feiertag_faktor;
						$soll_sek_prozent=$soll_sekunden;

						$msg=0;
						$notice[0]=array(0 => $this->is_valid_person($person, $email_addr[$pid], $wday));
						if($notice[0][0] == 0)
						{
							$fehlzeit=$this->get_fehlzeit($fehl_termine[$pid], $daily_workload);
							$ist_zeiten[$pid]['zeitaufwand'] += $fehlzeit;

							// Ist-Zeitaufwand < Soll-Zeitaufwand => Meldung dass Stunden fehlen
							$diff_prozent=$soll_sek_prozent - $ist_zeiten[$pid]['zeitaufwand'];
							if($diff_prozent > 0)
							{
								// Zeitaufwand: Es fehlen noch mindestens $var1 Stunde(n).
								$notice[$msg]=array($this->TXT_ZEITAUFWAND, ': '.str_replace('$var1', get_entry($diff_prozent/3600, 'dezimal'), $this->get_task_message($this->TXT_ERR_HOURS)));
								$msg++;
							}
							else
							{
								// Alles ok! Zeiterfassung und Pause wurden vorschriftsmässig eingetragen.
								$notice[$msg]=array($this->TXT_AOK);
							}

						}

						// Nur Warnmeldungen dem Vorgesetzten weiterleiten
						if($msg)
							$this->add_warning($person, $notice, $ist_zeiten[$pid]['zeitaufwand'], $soll_sekunden);
					}
				}
			}

			if(sizeof($this->elter_warning) > 0)
			{
				$elter_ids=array_keys($this->elter_warning);
				$elter_daten=$this->get_elter_daten($elter_ids);
				$elter_mail_adresses=get_email_adress(implode(', ', $elter_ids));

				foreach($this->elter_warning as $elter => $warning)
				{
					if(is_null($elter_mail_adresses[$elter]['email']) || !strlen((string)$elter_mail_adresses[$elter]['email']))
						continue;

					setUserSettingsScheduler($elter, $this->mandanten_id);
					$this->show('<div style="border:1px dotted #888888;border-radius:6px;padding:4px;margin-bottom:2px;">');

					// Name des Vorgesetzen zusammenbauen
					$elter_name=trim((string)$elter_daten[$elter]['vorname'].' '.$elter_daten[$elter]['pname']);
					$this->show('Supervisor <b>'.$elter.'</b> <i>&quot;'.$elter_name.'&quot;</i><br />');

					$rsent=array();
					$cnt=1;
					while(true)
					{
						$mt  = '			<table width="100%" border="0" cellpadding="2" cellspacing="1" bgcolor="#bbbbbb">'.$GLOBALS['nl'];
						$prev_pid=0;
						foreach($warning as $pid => $user_warning)
						{
							if(!isset($rsent[$pid]))
							{
								// Leerzeile einfügen wenn neue Person
								if($prev_pid && ($prev_pid != $pid))
									$mt .= '				<tr><td colspan="5"></td></tr>'.$GLOBALS['nl'];

								$mlen=strlen((string)$mt)+strlen((string)$user_warning);
								if($mlen < 40000)
								{
									$mt .= $user_warning;
									$prev_pid=$pid;
									$rsent[$pid]=true;
								}
								else
									break;
							}
						}
						$mt .= '			</table>'.$GLOBALS['nl'];

						// Ergebnis des Zeiterfassungschecks der zu "$var1" zugeordneten Mitarbeiter im Zeitraum vom $var2 bis zum $var3.
						$txt=$this->get_task_message($this->TXT_ELTER_MAIL_HEADER);
						$txt=str_replace(array('$var1', '$var2', '$var3'), array($elter_name, get_entry($start,'datum'), get_entry($end, 'datum')), $txt);

						// Mailinhalt erstellen
						$mc  = '<table width="80%" border="0" cellpadding="2" cellspacing="1" bgcolor="#cccccc" style="border-radius:4px;">'.$GLOBALS['nl'];
						$mc .= '	<tr>'.$GLOBALS['nl'];
						$mc .= '		<td width="100%" bgcolor="#e1001a" style="border-top-left-radius:4px;border-top-right-radius:4px;border-bottom:1px solid #222222;"><b style="color:#ffffff;'.$GLOBALS['class_fontstyle'].'">'.$txt.'</b></td>'.$GLOBALS['nl'];
						$mc .= '	</tr>'.$GLOBALS['nl'];
						$mc .= '	<tr>'.$GLOBALS['nl'];
						$mc .= '		<td>'.$GLOBALS['nl'];
						$mc .= $mt;
						$mc .= '		</td>'.$GLOBALS['nl'];
						$mc .= '	</tr>'.$GLOBALS['nl'];
						$mc .= '</table>'.$GLOBALS['nl'];

						// Mail an Vorgesetzten senden
						$send_error=send_mail($elter, $this->get_task_message($this->TXT_ZEIT_CHK).' '.$cnt, $mc);
//fxDebug($send_error, '$send_error: '.$elter.' (strlen='.strlen((string)$mc).')');
						if(!$send_error)
							$send_error='OK';
						if($this->debug == $this->prog_id)
							$this->show('<div style="padding:6px;background:#fff;border:1px solid #ccc;box-shadow:3px 3px 2px rgba(0,0,0, 0.25);">'.$mc.'</pre></div>');
						$this->show('&nbsp;&nbsp;&nbsp;&middot; <i>Send mail returned [<b>'.$send_error.'</b>]</i>');

						unset($hf);
						$cnt++;

						// Falls alle gesendet wurden, dann Schleife verlassen
						if(sizeof($rsent) >= sizeof($warning))
							break;
					}
					$this->show('</div>');
				}
			}
		}
		else
			$this->show('<b>Check supervisors:</b><br />&nbsp;&nbsp;&nbsp;&middot; <font style="color:#888888;">Check inactive.</font>');
		$this->show('<hr style="border:0;border-top:1px solid #888888;" />');
	}

	/**
	 * ermittelt für die übergebenen Elter die Personendaten
	 * @param array $elter_ids Array das die Personen-IDs der Elter enthält
	 * @return array das Array mit den Daten zu den Eltern (Name, Vorname, Anrede)
	 * [1086] => array('pname' => 'Binder', 'vorname' => 'Walter', 'anrede' => '311')
	 */
	function get_elter_daten($elter_ids)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT personen_id, anrede, pname, vorname FROM personen";
		$sql .= " WHERE personen_id IN (".implode(',', $elter_ids).") AND mandanten_id=".$this->mandanten_id;
		$values=db_values($sql, '*');
//fxDebug($values,$sql, 0);

		$parents=array();
		if(fxIsArray($values))
		{
			foreach($values as $va)
				$parents[$va['personen_id']]=$va;
		}
//fxDebug($parents,'$parents', 0);

		return $parents;
	}

	/**
	 * prüft ob der Elter-Check für heute aktiviert ist,
	 * und ermittelt wann er zu letzt ausgeführt wurde
	 * @return int Tage seit dem der Elter-Check zuletzt ausgeführt wurde,
	 * 0 falls keine Ausführung nötig
	 */
	function is_elter_chk_active()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT einstellungswert FROM s_einstellungswerte";
		$sql .= " WHERE benutzer_id=1 AND einstellungs_id=32 AND mandanten_id IN (0,".$this->mandanten_id.") ORDER BY mandanten_id DESC";
		$value=db_value($sql);
//fxDebug($value,$sql, 0);

		$setting=trim((string)str_replace(' ', '', $value));
		// keine Tage für Elter-Summary gesetzt => Summary für Elter nicht erstellen
		if(is_null($setting) || !strlen((string)$setting))
			return 0;
		else
		{
			$days=explode(',', $setting);
			// heutiger Tag nicht in Array => Summary für Elter nicht erstellen
			if(($key = array_search(date('w', time()), $days)) === false)
				return 0;
			else
			{
				sort($days);
				if($key == 0)
					$prev_day=$days[count($days) - 1] - 7;
				else
					$prev_day=$days[$key - 1];
				return abs($days[$key] - $prev_day);
			}
		}
	}

	/**
	 * fügt dem elter_warning array eine neue Warnung hinzu
	 * @param array $person Array mit den Personen-Daten
	 * @param array $notice Array, dass die Warnung enthält
	 * @param double $ist_aufwand der Ist-Aufwand der Person
	 * @param double $soll_aufwand der Soll-Aufwand der Person
	 */
	function add_warning($person, $notice, $ist_aufwand, $soll_aufwand)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if(!is_null($person['elter']) && strlen((string)$person['elter']))
		{
			if(!isset($this->elter_lines[$person['elter']]))
				$this->elter_lines[$person['elter']]=array();
			if(!isset($this->elter_lines[$person['elter']][$person['personen_id']]))
				$this->elter_lines[$person['elter']][$person['personen_id']]=0;
			$pc=$this->elter_lines[$person['elter']][$person['personen_id']];

			$ca=array(array('#dddddd','#f8f8f8'), array('#d8d8d8','#f1f1f1'));
			$dc=$ca[$pc%2][0];
			$lc=$ca[$pc%2][1];
			if(!$pc)
			{
				// Zeile: Name
				$this->elter_warning[$person['elter']][$person['personen_id']]  = '				<tr>'.$GLOBALS['nl'];
				$this->elter_warning[$person['elter']][$person['personen_id']] .= '					<td nowrap bgcolor="#006b9f" colspan="5" style="border-top:1px solid #eeeeee;font-size:12pt;">&nbsp;<b style="color:#dddddd;">'.$person['vorname'].' '.$person['pname'].'</b>&nbsp;</td>'.$GLOBALS['nl'];
				$this->elter_warning[$person['elter']][$person['personen_id']] .= '				</tr>'.$GLOBALS['nl'];

				// Zeile: Überschrift
				$this->elter_warning[$person['elter']][$person['personen_id']] .= '				<tr>'.$GLOBALS['nl'];
				$this->elter_warning[$person['elter']][$person['personen_id']] .= '					<td nowrap bgcolor="#aaaaaa" colspan="2">&nbsp;<b>'.$this->get_task_message($this->TXT_DATUM).'</b>&nbsp;</td>'.$GLOBALS['nl'];
				$this->elter_warning[$person['elter']][$person['personen_id']] .= '					<td nowrap bgcolor="#aaaaaa" colspan="2">&nbsp;<b>'.$this->get_task_message($this->TXT_ZEIT_IST_SOLL).'</b>&nbsp;</td>'.$GLOBALS['nl'];
				$this->elter_warning[$person['elter']][$person['personen_id']] .= '					<td nowrap bgcolor="#aaaaaa" width="100%">&nbsp;<b>'.$this->get_task_message($this->TXT_AKTION).'</b>&nbsp;</td>'.$GLOBALS['nl'];
				$this->elter_warning[$person['elter']][$person['personen_id']] .= '				</tr>'.$GLOBALS['nl'];
			}

			// Zeile: Nachricht
			$message=$this->get_task_message($notice[0][0]).$notice[0][1];

			$col=$this->get_color($notice[0][0]);
			if($col == 'dddddd')
			{
				$txt_ot='<i style="color:#'.$col.';">';
				$txt_ct='</i>';
			}
			else
			{
				$txt_ot='<font style="color:#'.$col.';">';
				$txt_ct='</font>';
			}

			$weekday=fx_date('w', fx_mktime(12, 0, 0, (int)substr((string)$this->date00, 4, 2), (int)substr((string)$this->date00, 6, 2), (int)substr((string)$this->date00, 0, 4)));

			$this->elter_warning[$person['elter']][$person['personen_id']] .= '				<tr>'.$GLOBALS['nl'];
			// ...Wochentag
			$this->elter_warning[$person['elter']][$person['personen_id']] .= '					<td bgcolor="'.$dc.'" nowrap>&nbsp;'.$this->get_weekday($weekday).'&nbsp;</td>'.$GLOBALS['nl'];
			// ...Datum
			$this->elter_warning[$person['elter']][$person['personen_id']] .= '					<td bgcolor="'.$dc.'" nowrap>'.get_entry($this->date00, 'datum').'&nbsp;</td>'.$GLOBALS['nl'];
			// ...Ist-Zeit
			$this->elter_warning[$person['elter']][$person['personen_id']] .= '					<td bgcolor="'.$lc.'" align="right" nowrap>&nbsp;'.get_entry($ist_aufwand / 3600, 'dezimal').'&nbsp;'.$this->get_task_message($this->TXT_STUNDEN).'&nbsp;</td>'.$GLOBALS['nl'];
			// ...Soll-Zeit
			$this->elter_warning[$person['elter']][$person['personen_id']] .= '					<td bgcolor="'.$lc.'" align="right" nowrap>&nbsp;'.get_entry($soll_aufwand / 3600, 'dezimal').'&nbsp;'.$this->get_task_message($this->TXT_STUNDEN).'&nbsp;</td>'.$GLOBALS['nl'];
			// ...Aktion
			$this->elter_warning[$person['elter']][$person['personen_id']] .= '					<td bgcolor="'.$lc.'" nowrap>&nbsp;'.$txt_ot.$message.$txt_ct.'</td>'.$GLOBALS['nl'];
			$this->elter_warning[$person['elter']][$person['personen_id']] .= '				</tr>'.$GLOBALS['nl'];

			$this->elter_lines[$person['elter']][$person['personen_id']]++;
		}
	}

	/**
	 * ermittelt die Hintergrundfarbe für eine Elter-Warnung
	 * @return string der HTML-Farbcode der Hintergrundfarbe
	 */
	function get_color($message_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		// TXT_AOK:				Alles ok! Zeiterfassung und Pause wurden vorschriftsmässig eingetragen.
		if($message_id == $this->TXT_AOK)
			return '009f6b';
		// TXT_ZEITAUFWAND:		Zeitaufwand
		else if($message_id == $this->TXT_ZEITAUFWAND)
			return 'e1001a';
		// TXT_EMP_NO_AUX_DATA:	Dieser Mitarbeiter besitzt keine Zusatzdaten!
		// TXT_EMP_NO_CHK:		Bei diesem Mitarbeiter ist der Zeiterfassungscheck in den Zusatzdaten nicht aktiviert!
		// TXT_EMP_NO_MAIL:		Diesem Mitarbeiter ist noch kein Benutzer mit einer gültigen E-Mail-Adresse zugeordnet!
		// TXT_EMP_NO_WORKDAY:	Dieser Mitarbeiter muss an diesem Tag nicht arbeiten!
		// TXT_EMP_APPOINTMENT:	Dieser Mitarbeiter ist an diesem Tag wegen eines Termins entschuldigt.
		else
			return 'dddddd';
	}

	/**
	 * ermittelt das Personal des Mandanten das aktiv ist und auch Arbeitstage definiert hat
	 * @param string $start wenn angegeben, wird dieses Datum als Grenz-Eintrittdatum verwendet,
	 * sonst wird date00 verwendet 
	 * @param string $end wenn angegeben, wird dieses Datum als Grenz-Austrittdatum verwendet,
	 * sonst wird date24 verwendet
	 * @return array das Array mit den Personendaten
	 */
	function get_personal($start='', $end='')
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if(is_null($start) || !strlen((string)$start))
			$start=$this->date00;
		if(is_null($end) || !strlen((string)$end))
			$end=$this->date24;

		$sql  = "SELECT p.personen_id, p.personentyp, p.pname, p.vorname, p.anrede, p.elter, p.abt_id, p.geschlecht, p.abt_id,";
		$sql .= " m.wochenarbeitstd, m.arbeitstage, m.soll_beginn, m.zeiterfassung, m.pause, m.maeintrittsdatum, m.maaustrittsdatum, m.montag, m.dienstag, m.mittwoch, m.donnerstag, m.freitag, m.samstag, m.sonntag";
		$sql .= " FROM personen p LEFT JOIN madaten m ON p.personen_id=m.personen_id";
		$sql .= " WHERE p.mandanten_id=".$this->mandanten_id." AND m.mandanten_id=".$this->mandanten_id." AND p.personen_id>1";
		$sql .= " AND (p.personentyp=".FXP_PT_EMPLOYEE." OR p.personentyp=".FXP_PT_CONTRACTOR.") AND (p.inaktiv=0 OR p.inaktiv IS NULL)";
		$sql .= " AND (p.gueltig_von IS NULL OR p.gueltig_von<='".$end."') AND (p.gueltig_bis IS NULL OR p.gueltig_bis>='".$start."')";
		$sql .= " AND m.maeintrittsdatum<='".$end."' AND (m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>='".$start."') AND m.wochenarbeitstd>0 AND m.arbeitstage>0";
		$sql .= " ORDER BY p.pname, p.vorname";
		$values=db_values($sql,'*');
//fxDebug($values,$sql, 0);

		$persons=array();
		if(fxIsArray($values))
		{
			foreach($values as $va)
				$persons[$va['personen_id']]=$va;
		}
//fxDebug($persons,'$persons', 0);

		return $persons;
	}

	/**
	 * überprüft, ob für eine Person der Zeiterfassungscheck überhaupt sinnvoll
	 * durchgeführt werden kann
	 * @param array $person das Array mit den Personendaten
	 * @param array $email_addr das Array das die eMail-Adresse der Person enthalten sollte
	 * @param string $wday der aktuelle Wochentag, als kleingeschriebener deutscher Name
	 * @return 0, falls okay, sonst ID des Fehlercodes bzw. der daraus resultierenden Meldung
	 */
	function is_valid_person($person, $email_addr, $wday)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		// keine Zusatzdaten ... nächste Person
		if(!$person['wochenarbeitstd'])
			return $this->TXT_EMP_NO_AUX_DATA;
		// keine Mailadresse ... nächste Person
		else if(is_null($email_addr['email']) || !strlen((string)$email_addr['email']))
			return $this->TXT_EMP_NO_MAIL;
		// kein Zeiterfassungs-Check bei dieser Person ... nächste Person
		else if(!$person['zeiterfassung'])
			return $this->TXT_EMP_NO_CHK;
		// keine Arbeitstag ... nächste Person
		else if(!$person[$wday])
			return $this->TXT_EMP_NO_WORKDAY;
		else
			return 0;
	}

	/**
	 * liefert die Ist-Zeiten zu einem Array von Personen-IDs
	 * @param array $arr_pers_id das Array mit den Personen-IDs
	 * @return array das Array mit den Ist-Zeiten, Key ist die jeweilige Personen-ID
	 */
	function get_ist_zeiten($arr_pers_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT personen_id, SUM(zeitaufwand) AS zeitaufwand, SUM(pause) AS pause, MIN(ist_beginn) AS ist_beginn, MAX(ist_ende) AS ist_ende";
		$sql .= " FROM zeitdaten";
		$sql .= " WHERE personen_id IN (".implode(',', $arr_pers_id).") AND mandanten_id=".$this->mandanten_id;
		$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND (ist_beginn<='".$this->date24."' AND ist_ende>='".$this->date00."')";
		$sql .= " GROUP BY personen_id";
		$values=db_values($sql,'*');
//fxDebug($values,$sql, 0);

		$times=array();
		if(fxIsArray($values))
		{
			foreach($values as $va)
				$times[$va['personen_id']]=$va;
		}
//fxDebug($times,'$times', 0);

		return $times;
	}

	/**
	 * liefert die Fehltermine zu einem Array von Personen-IDs
	 * @param array $arr_pers_id das Array mit den Personen-IDs
	 * @return array das Array mit den Fehlterminen, Key ist die jeweilige Personen-ID
	 */
	function get_fehl_termine($arr_pers_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$fehl_term=array();

		$sql  = "SELECT personen_id, terminart, startzeit, endzeit FROM termine";
		$sql .= " WHERE personen_id IN (".implode(',', $arr_pers_id).") AND mandanten_id=".$this->mandanten_id;
		$sql .= " AND (startzeit<='".$this->date24."' AND endzeit>='".$this->date00."')";
		$sql .= " AND terminart IN (".$GLOBALS['_tca'][FXP_AC_ABSENT].")";
		$values=db_values($sql,'*');
//fxDebug($values,$sql, 0);

		if(fxIsArray($values))
		{
			foreach($values as $fehl_termin)
			{
				$pid=$fehl_termin['personen_id'];
				if(!isset($fehl_term[$pid]))
					$fehl_term[$pid]=array();
				$fehl_term[$pid][]=$fehl_termin;
			}
		}

		return $fehl_term;
	}

	/**
	 * liefert zu einem Fehltermin die jeweilige Fehlzeit in Sekunden
	 * @param array $fehl_termine das Array das die Informationen zu dem Fehltermin enthält
	 * @param int $daily_workload die Soll-Arbeitsdauer des Mitarbeiters in Sekunden
	 * @return int die Fehlzeit in Sekunden die durch diesen Fehltermin entstanden
	 * bei einem mehrtägigem Fehltag = die Soll-Arbeitsdauer des Mitarbeiters in Sekunden
	 * sonst Differenz aus Endezeit - Startzeit in Sekunden
	 */
	function get_fehlzeit($fehl_termine, $daily_workload)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if(!is_array($fehl_termine))
			return 0;
		if((count($fehl_termine[$person['personen_id']]) > 1 ) || (getDateString($fehl_termine[0]['startzeit']) != getDateString($fehl_termine[0]['endzeit'])))
			return $daily_workload;
		else
			return zeitsek($fehl_termine[0]['endzeit']) - zeitsek($fehl_termine[0]['startzeit']);
	}

	/**
	 * initialisiert das interne messages-Array mit den nötigen Meldungen,
	 * Key ist die ID der Meldung, negiert falls es eine Meldung aus der Tabelle
	 * "texte" ist
	 */
	function init_messages()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$this->messages=array();

		$sql  = "SELECT wertetabid, id_sprache, tabwert AS text FROM wertetab";
		$sql .= " WHERE (wertetabid IN (234,235,236,1701,1702,1703,1704,1705,1706,1707,1708,1710,1711,1712,2700) OR id_feld IN (17,30,330,987,1209)) AND mandanten_id=0";
		$values=db_values($sql,'*');
//fxDebug($values,$sql, 0);
		if(fxIsArray($values))
		{
			foreach($values as $va)
			{
				if(!isset($this->messages[$va['wertetabid']]))
					$this->messages[$va['wertetabid']]=array();
				$this->messages[$va['wertetabid']][$va['id_sprache']]=$va['text'];
			}
		}

		$sql  = "SELECT -id_referenz AS id_referenz, id_sprache, literal_15 AS text FROM texte";
		$sql .= " WHERE id_referenz IN (14,274,310,332,852) AND id_ref_art=3";
		$values=db_values($sql,'*');
//fxDebug($values,$sql, 0);

		if(fxIsArray($values))
		{
			foreach($values as $va)
			{
				if(!isset($this->messages[$va['id_referenz']]))
					$this->messages[$va['id_referenz']]=array();
				$this->messages[$va['id_referenz']][$va['id_sprache']]=$va['text'];
			}
		}
	}

	/**
	 * initialisiert das interne weekdays-Array mit den Wochentagskurznamen,
	 * Key ist: 0=So, 1=Mo, ..., 6=Sa (entspr. PHP)
	 * Value ist: Wochentagkurzname
	 */
	function init_weekdays()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql = "SELECT position, tabwert, id_sprache FROM wertetab WHERE id_feld=289 AND mandanten_id=0";
		$tmp = db_values($sql, '*');

		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$p = (int)$t['position'];
				$l = (int)$t['id_sprache'];
				$w = substr((string)$t['tabwert'], 0, 2);

				if(!isset($this->weekdays[$p]))
					$this->weekdays[$p] = array();
				$this->weekdays[$p][$l] = $w;
			}
		}
	}

	/**
	 * liefert eine Meldung aus dem internen messages-Array
	 * @param int $message_id die ID der Meldung, negiert falls es eine Meldung
	 * aus der Tabelle "texte" ist
	 * @return unknown
	 */
	function get_task_message($message_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		if(fxIsArray($this->messages) && fxIsArray($this->messages[$message_id]))
			return $this->messages[$message_id][$GLOBALS['fxpglobals']['lang']];
		return '['.$message_id.']';
	}

	/**
	 * liefert einen Wochentagkurznamen aus dem internen weekdays-Array
	 * @param int $weekday: 0=So, 1=Mo, ..., 6=Sa
	 * @return string Wochentagkurzname des Tages in der aktuellen Sprache
	 */
	function get_weekday($weekday)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$weekday = min(6, max(0, (int)$weekday));
		if(is_array($this->weekdays) && is_array($this->weekdays[$weekday]))
			return $this->weekdays[$weekday][$GLOBALS['fxpglobals']['lang']];
		return '??';
	}

	/**
	 * ermittelt den Feiertagsfaktor
	 * @return double 0, falls zum internen Datum date00 ein ganztägiger Feiertag ist
	 * 0.5 bei einem halben Feiertag, 0 wenn kein Feiertag ist
	 */
	function init_feiertag_faktor()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT feiertag_name, halbtaegig FROM feiertage_mand";
		$sql .= " WHERE mandanten_id=".$this->mandanten_id." AND datum='".$this->date00."'";
		$values=db_values($sql);
//fxDebug($values,$sql, 0);

		if(!is_array($values))
			return 1;

		if($values['halbtaegig'] == FXP_HL_FULL)
			return 0;
		else if($values['halbtaegig'] == FXP_HL_HALF)
			return 0.5;
		else
			return 1;
	}

	/**
	 * ermittelt die Soll-Prozent Angabe die unterschritten werden muss, um eine
	 * Warnmeldung des Zeiterfassungs-Checks bei einem Mitarbeiter zu verursachen
	 * @return double die Soll-Prozent Angabe zwischen 0 und 1
	 */
	function init_soll_prozent()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$sql  = "SELECT einstellungswert FROM s_einstellungswerte";
		$sql .= " WHERE benutzer_id=1 AND einstellungs_id=31 AND mandanten_id IN (0,".$this->mandanten_id.")";
		$sql .= " ORDER BY mandanten_id DESC";
		$value=db_value($sql);
//fxDebug($value,$sql, 0);

		return max(0, min(1, $value/100.0));
	}
}

/**
 * @author Stefan Löwe
 * Klasse zum Ausführen des Mailings
 */
class eMailTask extends Task
{
	// Flag aus d_dok_transfer_log wenn eine Mail versendet wurde
	var $sentsearch = "'64-%'";
	var $sentvalue = "'64-EMS-1,00'";
	// Endung von eMail-Dateien
	var $type = "'*.eml'";


	////////////////////////////////////////////////////////////////////////////////
	// METHODS
	////////////////////////////////////////////////////////////////////////////////

	/**
	 * Konstruktor der Klasse
	 * @param int $prog_id die Programm-ID der Task die erstellt werden soll
	 * @param int $mandanten_id die ID des Mandanten für den die Task ausgeführt werden soll
	 */
	public function __construct($prog_id, $mandanten_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		parent::__construct($prog_id, $mandanten_id);
	}

	/**
	 * sendet im DMS gespeicherte und noch nicht gesendete Mails für alle Mandanten
	 * @return boolean true, wenn ein Fehler auftrat, false sonst
	 */
	function sendDMSMails()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$err=false;
		$db_error=false;
		$ml_error=false;
		$undeliveredMails=$this->getUndeliveredMails();
		if(fxIsArray($undeliveredMails))
		{
			if(isset($GLOBALS['fxpglobals']) && is_array($GLOBALS['fxpglobals']) && isset($GLOBALS['fxpglobals']['settings']) && is_array($GLOBALS['fxpglobals']['settings']) && isset($GLOBALS['fxpglobals']['settings']['dok_archiv']) && !is_null($GLOBALS['fxpglobals']['settings']['dok_archiv']) && strlen((string)$GLOBALS['fxpglobals']['settings']['dok_archiv']))
				$path=$GLOBALS['fxpglobals']['settings']['dok_archiv'];
			else
			{
				$sql="SELECT einstellungswert FROM s_einstellungswerte WHERE einstellungs_id=18 AND mandanten_id=1 AND benutzer_id=1";
				$path=db_value($sql);
			}
			$path=str_replace("\\",'/',$path);
			if(substr((string)$path,-1) != '/')
				$path .= '/';
			$path .= $GLOBALS['_mail_mandantenid'].'/';
			$this->show('&middot; <b>Path:</b> '.$path.'<br />');

			$mail_cnt=sizeof($undeliveredMails);
			foreach($undeliveredMails as $cnt => $undeliveredMail)
			{
				$this->show('&middot; <b>Undelivered email #'.($cnt+1).':</b> '.$undeliveredMail['pfad'].' <i style="color:#888888;">('.$undeliveredMail['dok_id'].', &quot;'.$undeliveredMail['dok_originalname'].'&quot;)</i> &hellip; ');

				// Check, if mail document exists
				$mail_file=$path.$undeliveredMail['pfad'];
				if(file_exists($mail_file))
				{
					// DMS-E-Mail versenden
					$error=send_dms_mail($mail_file, 0);

					// Mails SOFORT nach Senden als gesendet markieren, da sonst der nächste Start des Schedulers evtl. die Mail nochmal versenden würde
					if(!$error)
					{
						$this->show('<b style="color:#009f6b;">OK</b>');
						$db_error=$this->setMailAsSent($undeliveredMail['dok_id'], $GLOBALS['_mail_mandantenid']);
						if($db_error)
							$this->show(' -&gt; <i style="color:#e1001a;">Email status could not be set as SENT</i>');
					}
					else
					{
						$this->show('<b style="color:#e1001a;">ERROR</b> <i style="color:#e1001a;">&quot;'.$error.'&quot; &rarr; Email could not be sent!</i>');
						$ml_error=true;
					}
				}
				else
				{
					$this->show('<b style="color:#e1001a;">ERROR</b> <i style="color:#e1001a;">Email file does not exist anymore in the DMS folder!</i>');
					$ml_error=true;
				}
				$this->show('<br />');

				// Falls  DB- oder Mail-Fehler auftritt, nicht abbrechen, aber Fehler trotzdem melden
				if($db_error || $ml_error)
					$err=true;

				// Aktivität melden, bzw. wenn letzte Mail erreicht ist, Bearbeitungsende melden
				$this->report_activity($mail_cnt == $cnt + 1);
			}
		}
		else
			$this->show('&middot; <i>No undelivered emails found</i><br />');
		return $err;
	}

	/**
	 * liefert alle noch nicht versendeten Mails des Mandanten
	 * return array das Array mit den noch nicht versendeten Mails des/aller Mandanten 
	 */
	function getUndeliveredMails()
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		// liefert alle erzeugten eMails, außer der bereits versendeten
		$sql  = "SELECT dv.mandanten_id, dv.dok_id, d.dok_originalname, dv.datei_name, dv.pfad, dv.zeitstempel";
		$sql .= " FROM d_dok_verlauf dv, d_dokumente d";
		$sql .= " WHERE d.elter_art=3 AND d.dok_typ=".$this->type." AND d.mandanten_id=".$this->mandanten_id;
		$sql .= " AND d.dok_id=dv.dok_id AND dv.mandanten_id=".$this->mandanten_id;
		$sql .= " AND dv.dok_id NOT IN (SELECT dok_id FROM d_dok_transfer_log WHERE mandanten_id=".$this->mandanten_id." AND transfer_art LIKE ".$this->sentsearch.")";
		$sql .= " ORDER BY dv.zeitstempel, dv.dok_id";
		$arr=db_values($sql, '*');
//fxDebug($arr,$sql, 0);

		return $arr;
	}

	/**
	 * setzt für eine Mail das gesendet-Flag in d_dok_transfer_log
	 * @param int $dok_id Array die 'dok_id' der Mail
	 * @return boolean true wenn ein Fehler auftrat, false sonst
	 */
	function setMailAsSent($dok_id, $mandanten_id=0)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		$err=false;
		$dok_id=(int)$dok_id;
		$mandanten_id=(int)$mandanten_id;
		if($mandanten_id <= 0)
			$mandanten_id=$GLOBALS['fxpglobals']['client'];
		if($dok_id > 0)
		{
			$sql  = "INSERT INTO d_dok_transfer_log (mandanten_id, dok_log_id, dok_id, transfer_art)";
			$sql .= " VALUES (".$mandanten_id.", ".id_max('d_dok_transfer_log', 'dok_log_id').", ".$dok_id.", '64-EMS-1,00')";
			$err=db_x($sql);
		}
		return $err;
	}
}

/**
 * @author Stefan Löwe
 * Klasse zum Ausführen der Warnungen
 */
class WarningTask extends Task
{
	////////////////////////////////////////////////////////////////////////////////
	// METHODS
	////////////////////////////////////////////////////////////////////////////////

	/**
	 * Konstruktor der Klasse
	 * @param int $prog_id die Programm-ID der Task die erstellt werden soll
	 * @param int $mandanten_id die ID des Mandanten für den die Task ausgeführt werden soll
	 */
	public function __construct($prog_id, $mandanten_id)
	{
		// Display function call?
		if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

		parent::__construct($prog_id, $mandanten_id);
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ermittelt den deutschen Namen der Wochentage, zum ermitteln der
 * Spaltennamen der scheduler_settings-Tabelle
 * @param int $weekday der Wochentag als Zahl kodiert, 0 = Sonntag
 * @return string der Name des entsprechenden Wochentags 
 */
function getNameOfWeekday($weekday)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$weekday = min(6, max(0, (int)$weekday));
	$wname = db_value("SELECT tabwert FROM wertetab WHERE id_feld=289 AND position=".$weekday." AND id_sprache=1 AND mandanten_id=0");

	return $wname;
}

/**
 * ermittelt zu einem Date-Time-String den Stunden- und Minutenanteil, der Rest
 * wird mit Nullen aufgefüllt
 * @param string $date der Date-Time-String
 * @return string ein String mit Nullen außer an den Positionen von Stunden und Minuten
 */
function getTimeString($date)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return '00000000'.substr((string)$date, 8, 4).'00';
}

/**
 * ermittelt zu einem Date-Time-String den Datumsteil
 * @param string $date der Date-Time-String
 * @return string ein Date-Time-String ohne den Zeitanteil
 */
function getDateString($date)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return substr((string)$date, 0, 8);
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 * @param various $mid - Mandatory parameter: ???PARAMETER???
 */
function setUserSettingsScheduler($pid, $mid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=(int)$mid;
	if(!$pid)
		$pid=1;

	// Benutzer-ID der Person ermitteln
	$bid=db_value("SELECT benutzer_id FROM benutzer WHERE personen_id=".$pid." AND mandanten_id=".$mid);

	$sql_arr[0]="SELECT einstellungs_id, einstellungswert FROM s_einstellungswerte WHERE mandanten_id=0 AND benutzer_id=1 AND einstellungs_id IN (1,2,9,15,21)";

	if($mid)
		$sql_arr[1]="SELECT einstellungs_id, einstellungswert FROM s_einstellungswerte WHERE mandanten_id=".$mid." AND benutzer_id=1 AND einstellungs_id IN (1,2,9,15,21)";

	if($mid && $bid)
		$sql_arr[2]="SELECT einstellungs_id, einstellungswert FROM s_einstellungswerte WHERE mandanten_id=".$mid." AND benutzer_id=".$bid." AND einstellungs_id IN (1,2,9,15,21)";

	// Umschlüsselungs-Tabelle
	$idToName=array(1=>'sprache', 2=>'formatdatum', 9=>'deztrennzeichen', 15=>'waehrung', 21=>'nkstellen');
	foreach($sql_arr as $cnt => $sql)
	{
		$values=db_values($sql,'*');
		if(fxIsArray($values))
		{
			foreach($values as $val)
			{
				if(($val['einstellungs_id'] != 1) && ($val['einstellungs_id'] != 9))
					$val['einstellungswert']=meldung($val['einstellungswert']);
				$GLOBALS['fxpglobals']['settings'][$idToName[$val['einstellungs_id']]]=$val['einstellungswert'];
			}
		}
	}
//fxDebug($GLOBALS['fxpglobals']['settings'],'settings: $pid='.$pid.', $mid='.$mid, 0);
	$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];
}
?>