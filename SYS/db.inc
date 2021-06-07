<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : db.inc                                                       //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2021-03-22                                                   //
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
 * Function collection for handling all basic database routines
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////
$GLOBALS['db_in_trans']=false;
$GLOBALS['db_mia']=array();
if(isset($GLOBALS['sqlsrv_param_out']))
	unset($GLOBALS['sqlsrv_param_out']);


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Read and parse the fx-project configuration file into the variable $GLOBALS['fxpglobals']['dbparam']
 *
 * $GLOBALS['fxpglobals']['dbparam']['language']			Default login language
 *
 * $GLOBALS['fxpglobals']['dbparam']['oskey32']			 	OpenSSL random key 32 bytes
 * $GLOBALS['fxpglobals']['dbparam']['oskey64']			 	OpenSSL random key 64 bytes
 *
 * $GLOBALS['fxpglobals']['dbparam']['dbtype']				Database type
 * $GLOBALS['fxpglobals']['dbparam']['dbserver']			Server name
 * $GLOBALS['fxpglobals']['dbparam']['dbname']				Database name
 * $GLOBALS['fxpglobals']['dbparam']['username']			Database user
 * $GLOBALS['fxpglobals']['dbparam']['password']			Database user's password
 * $GLOBALS['fxpglobals']['dbparam']['dbprotocol']			Write database log?
 *
 * $GLOBALS['fxpglobals']['dbparam']['smtp_host']			SMTP host (for emails send over a socket conenction)
 * $GLOBALS['fxpglobals']['dbparam']['smtp_port']			SMTP port
 * $GLOBALS['fxpglobals']['dbparam']['smtp_helo']			HELO identifier
 * $GLOBALS['fxpglobals']['dbparam']['smtp_email']			Valid email address
 * $GLOBALS['fxpglobals']['dbparam']['smtp_account']		Valid email account (user)
 * $GLOBALS['fxpglobals']['dbparam']['smtp_password']		Valid email authorization password
 *
 * $GLOBALS['fxpglobals']['dbparam']['curl_proxy']			IP address for cURL proxy
 * $GLOBALS['fxpglobals']['dbparam']['curl_proxy_port']		Port for cURL proxy
 * $GLOBALS['fxpglobals']['dbparam']['curl_proxy_username']	User for cURL proxy
 * $GLOBALS['fxpglobals']['dbparam']['curl_proxy_password']	Password for cURL proxy
 *
 * $GLOBALS['fxpglobals']['dbparam']['sysadmin']			Allow System-Administrator access?
 * $GLOBALS['fxpglobals']['dbparam']['elinks']				Allow external links to webpages?
 * $GLOBALS['fxpglobals']['dbparam']['schedulername']	 	Program name of the created scheduler
 * $GLOBALS['fxpglobals']['dbparam']['scheduler']			Allow debug call of the scheduler?
 * $GLOBALS['fxpglobals']['dbparam']['adminpassword']		Administrator's password for special programs
 * $GLOBALS['fxpglobals']['dbparam']['email']				Send emails?
 *
 * @param boolean $connect - Optional parameter (default = true):  Open a database connection?
 *
 * @return Success/Error - false on success, else array containg error message(s)
 */
function db_ini($connect=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$filename=fxf_fxIniFile();
	if(!strlen($filename))
	{
		require('fxp_ini.inc');
		$filename=fxf_fxIniFile();
	}
	if(!strlen($filename))
		return 'ini';	// Ini erstellen

	$ret_error=array();
	$ret_replace=array('<!--#FILENAME#-->'=>'&quot;<b>'.$filename.'</b>&quot;');

	if(!fxIsArray($GLOBALS['fxpglobals']['dbparam']) || (isset($GLOBALS['fxpglobals']['dbparam']['timestamp']) && ($GLOBALS['fxpglobals']['dbparam']['timestamp'] != @filemtime($filename))))
	{
		$vdsa=array
		(
			'language'=>'i',
			'oskey32'=>'s', 'oskey64'=>'s',
			'dbtype'=>'i', 'dbserver'=>'s', 'dbname'=>'s', 'username'=>'c', 'password'=>'c', 'dbprotocol'=>'i',
			'smtp_host'=>'s', 'smtp_port'=>'i', 'smtp_helo'=>'s', 'smtp_email'=>'c', 'smtp_account'=>'c', 'smtp_password'=>'c',
			'curl_proxy'=>'s', 'curl_proxy_port'=>'i', 'curl_proxy_username'=>'c', 'curl_proxy_password'=>'c',
			'sysadmin'=>'i', 'elinks'=>'i', 'schedulername'=>'s', 'scheduler'=>'i', 'adminpassword'=>'s', 'email'=>'s'
		);

		$GLOBALS['fxpglobals']['dbparam']=array();
		$dpaa=array();

		// Read file line by line into an array (without empty lines)
		$fla=fxLoad($filename, 1);
		if(!fxIsArray($fla))
			$ret_error=array('title'=>'err_cf_title', 'text'=>'err_cf_empty_text');	// The configuration file <!--#FILENAME#--> cannot be opened or is empty!

		if(!sizeof($ret_error))
		{
			foreach($fla as $line)
			{
				$svp=0;
				$set='';
				$val='';
				$qas='';
				for($p=0; $p<strlen($line); $p++)
				{
					$c=substr($line,$p,1);
					if(ctype_print($c))
					{
						// Setting
						if(!$svp)
						{
							if(($c == '=') || ($c == ':'))
							{
								$set=trim($set);
								$svp=1;
							}
							else if(!ctype_space($c))
							{
								$set .= $c;
								if($set == '//')
								{
									$set='';
									break;
								}
							}
						}
						// Value
						else if(($c == '"') || ($c == "'"))
						{
							if($c == $qas)
								$qas='';
							else
								$qas=$c;
						}
						else if(strlen($qas))
							$val .= $c;
						else if(($c == '/') && (substr($line,$p+1,1) == '/'))
							break;
						else if(strlen($val) || !ctype_space($c))
							$val .= $c;
					}
				}

				if(strlen($set))
				{
					$set=strtolower($set);
//echo('$set=['.fxHtmlEncode($set).'], $val=['.fxHtmlEncode($val).']<hr />');
					if(isset($vdsa[$set]))
					{
						// Special check for "language", because numbers or strings are allowed
						if($set == 'language')
						{
							$dpaa[$set]=$val;
							$la=fxGetAllLanguages();
							if(ctype_digit($val))
								$val=(int)$val;
							else
							{
								$_ckn=strtolower(trim($val));
								$val=0;
								foreach($la as $lid => $laa)
								{
									if(($_ckn == $laa['cc']) || ($_ckn == strtolower($laa['tx'])))
									{
										$val=$lid;
										break;
									}
								}
							}
							if(isset($la[$val]))
							{
								$GLOBALS['fxpglobals']['dbparam'][$set]=(int)$val;
								unset($dpaa[$set]);
							}
						}
						// Special check for "database type", because numbers or strings are allowed
						else if($set == 'dbtype')
						{
							$dpaa[$set]=$val;
							if(ctype_digit($val))
								$val=(int)$val;
							else
							{
								$_ckn=strtolower(strtr($val, array(' '=>'', '-'=>'')));
								$val=0;
								foreach($GLOBALS['_dbtypes'] as $_dbt => $_dba)
								{
									foreach($_dba as $_dbn)
									{
										$_dbn=strtolower(strtr($_dbn, array(' '=>'', '-'=>'')));
										if($_ckn == $_dbn)
										{
											$val=$_dbt;
											break 2;
										}
									}
								}
							}
							if(isset($GLOBALS['_dbtypes'][$val]))
							{
								$GLOBALS['fxpglobals']['dbparam'][$set]=(int)$val;
								unset($dpaa[$set]);
							}
						}
						else if($vdsa[$set] == 'i')
							$GLOBALS['fxpglobals']['dbparam'][$set]=(int)$val;
						else
							$GLOBALS['fxpglobals']['dbparam'][$set]=$val;
					}
					else
						$dpaa[$set]=$val;
				}
			}

			// Entries found?
			if(sizeof($GLOBALS['fxpglobals']['dbparam']))
			{
				// Add additional entries?
				if(sizeof($dpaa))
					$GLOBALS['fxpglobals']['dbparam']['addon']=$dpaa;
				// Add file timestamp
				$GLOBALS['fxpglobals']['dbparam']['timestamp']=filemtime($filename);
			}
			else
			{
				$ret_error=array('title'=>'err_cf_title', 'text'=>'err_cf_invalid_text');	// The configuration file <!--#FILENAME#--> contains invalid entries!
				if(sizeof($dpaa))
				{
					$ret_error['detail']='<br /><br /><hr class="fxhr"><br /><br />';
					foreach($dpaa as $set => $val)
						$ret_error['detail'] .= '<b class=red>'.$set.'</b><br />';
				}
			}
		}
	}

	// Check if ini file is writeable
	if(!sizeof($ret_error) && !is_writeable($filename))
		$ret_error=array('title'=>'err_cf_title', 'text'=>'err_cf_wprotected_text');	// The configuration file <!--#FILENAME#--> is write protected!<br /><br /><hr class="fxhr"><br /><br />Please enable writing to this file for all groups<br />by disabling the write protection.

	// Check if we have our two openssl keys
	if(!sizeof($ret_error) && (!isset($GLOBALS['fxpglobals']['dbparam']['oskey32']) || !isset($GLOBALS['fxpglobals']['dbparam']['oskey64']) || !strlen($GLOBALS['fxpglobals']['dbparam']['oskey32']) || !strlen($GLOBALS['fxpglobals']['dbparam']['oskey64'])))
		$ret_error=array('title'=>'err_cf_title', 'text'=>'err_cf_oskey_text');	// Either the OpenSSL Key 32 or 64 in the configuration file <!--#FILENAME#--> is missing or empty!

	// Check for supported database type
	if(!sizeof($ret_error) && !isset($GLOBALS['fxpglobals']['dbparam']['dbtype']))
		$ret_error=array('title'=>'err_cf_title', 'text'=>'err_cf_dbtype_text');	// The configuration file <!--#FILENAME#--> contains either none<br />or an unsupported database type!
	else
		$GLOBALS['_dbtype']=$GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['type'];

	// Check if necessary PHP extensions are loaded
	if(!sizeof($ret_error))
	{
		$pext=fxf_checkExtensions($GLOBALS['fxpglobals']['dbparam']['dbtype']);
//fxDebug($pext,'$pext', 0);
		if(fxIsArray($pext) && strlen($pext['missing']))
		{
			$ret_error=array('title'=>'err_pi_title', 'text'=>'err_pi_missing_text');	// Dynamic PHP Extensions (libraries) missing!<br /><br /><hr class="fxhr"><br /><br />The following extensions are missing:<br /><br /><!--#EXTMISSING#--><br /><br /><br /><i>(Check the configuration in the &quot;php.ini&quot;<br />or recompile PHP with these extensions.)</i>
			$ret_replace['<!--#EXTMISSING#-->']='<b>'.$pext['missing'].'</b>';
		}
	}

	// Get php upload path
	if(!isset($GLOBALS['fxpglobals']['dbparam']['upload_dir']) || !strlen($GLOBALS['fxpglobals']['dbparam']['upload_dir']))
	{
		$utd=str_replace('\\', '/', ini_get('upload_tmp_dir'));
		if(substr($utd,-1) != '/')
			$utd .= '/';
		$GLOBALS['fxpglobals']['dbparam']['upload_dir']=$utd;
	}

	// No error and no db connection wanted or needed, leave now
	if(!sizeof($ret_error) && !$connect)
		return false;

	// Get database Id
	if(!sizeof($ret_error))
	{
		db_protokoll('create', false);
		$GLOBALS['dbid']=@db_oeffnen();
		if(!$GLOBALS['dbid'])
		{
			$ret_error=array('title'=>'err_db_title', 'text'=>'err_db_connect_text');	// A connection to the <!--#DBTYPE#-->-Database <!--#DBNAME#--> on the server <!--#DBSERVER#--><br />could not be established!
			$ret_replace['<!--#DBTYPE#-->']='<b>'.$GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['text'].'</b>';
			$ret_replace['<!--#DBNAME#-->']='<b>'.$GLOBALS['fxpglobals']['dbparam']['dbname'].'</b>';
			$ret_replace['<!--#DBSERVER#-->']='<b>'.$GLOBALS['fxpglobals']['dbparam']['dbserver'].'</b>';
		}
	}

	// The next parts are only executed if we have a database connection...
	if(!sizeof($ret_error) && $GLOBALS['dbid'])
	{
		// ...Get the database version and set additional parameters
		db_version();
		db_param();

		// ...Check if tables "tabellen", "benutzer" and "wertetab" exist (we asume everything else is ok if these three exist)
		$check_db=db_check('tabellen', 't');
		if($check_db)
			$check_db=db_check('benutzer', 't');
		if($check_db)
			$check_db=db_check('wertetab', 't');

		// ...If one of these tables are missing we need to install fx-project
		if(!$check_db)
			return 'inst';

		// Check database version and installation or update date: If values are empty we need to install fx-project
		$tmp=db_values("SELECT tabwert, zeitstempel FROM wertetab WHERE wertetabid=10 AND id_sprache=1 AND mandanten_id=0");
		if(fxIsArray($tmp))
		{
			$GLOBALS['fxpglobals']['dbparam']['version_db']=trim($tmp['tabwert']);
			while(substr($GLOBALS['fxpglobals']['dbparam']['version_db'],-2) == '.0')
				$GLOBALS['fxpglobals']['dbparam']['version_db']=trim(substr($GLOBALS['fxpglobals']['dbparam']['version_db'],0,-2));
			$GLOBALS['fxpglobals']['dbparam']['date_version']=$tmp['zeitstempel'];

			$versionscode='';
			if(strlen($GLOBALS['fxpglobals']['dbparam']['version_db']))
			{
				$vex=explode('.', $GLOBALS['fxpglobals']['dbparam']['version_db']);
				foreach($vex as $vcn)
					$versionscode .= substr('000'.$vcn, -3);
			}
			$GLOBALS['fxpglobals']['dbparam']['versionscode_db']=substr($versionscode.'000000000',0,9);
		}
		else
			return 'inst2';

		// Does our program version match the datbase version?
		// ...Update, if program > database
		if($GLOBALS['fxpglobals']['dbparam']['versionscode_prg'] > $GLOBALS['fxpglobals']['dbparam']['versionscode_db'])
			return 'upd';
		// ...Error, if version don't match
		else if($GLOBALS['fxpglobals']['dbparam']['versionscode_prg'] != $GLOBALS['fxpglobals']['dbparam']['versionscode_db'])
		{
			$ret_error=array('title'=>'err_vs_title', 'text'=>'err_vs_text');	// The current program version <!--#PVERSION#--> is smaller than<br />the current database version <!--#DVERSION#-->!
			$ret_replace['<!--#PVERSION#-->']='<b>'.$GLOBALS['fxpglobals']['dbparam']['version_prg'].'</b>';
			$ret_replace['<!--#DVERSION#-->']='<b>'.$GLOBALS['fxpglobals']['dbparam']['version_db'].'</b>';
		}

		// Get patch version and date
		if(!sizeof($ret_error))
		{
			$tmp=db_values("SELECT tabwert, zeitstempel FROM wertetab WHERE wertetabid=11 AND id_sprache=1 AND mandanten_id=0");
			if(fxIsArray($tmp))
			{
				$GLOBALS['fxpglobals']['dbparam']['version_patch']=$tmp['tabwert'];
				$GLOBALS['fxpglobals']['dbparam']['date_patch']=$tmp['zeitstempel'];
			}
		}

		// Create fieldname arrays for the following field types: MEMO, CHAR, FLOAT + INTEGER
		if(!sizeof($ret_error))
			fxf_dbFields();
	}
//fxDebug($GLOBALS['fxpglobals']['dbparam'],'dbparam', 0);

	// Error?
	if(sizeof($ret_error))
	{
		// Load initialization texts in selected language
		if(!fxIsArray($GLOBALS['_itla']))
			$GLOBALS['_itla']=fxGetLanguageTexts('init');
//fxDebug($GLOBALS['_itla'],'$GLOBALS[\'_itla\']: fxGetLanguageTexts(\'init\')',0);

		$ra=array(
			'tpath'=>$GLOBALS['_itla']['tpath_init'],
			'title'=>$GLOBALS['_itla'][$ret_error['title']],
			'text'=>$GLOBALS['_itla'][$ret_error['text']]
		);
		if(isset($ret_error['detail']))
			$ra['text'] .= $ret_error['detail'];
		if(sizeof($ret_replace))
			$ra['text']=strtr($ra['text'], $ret_replace);
		return $ra;
	}

	return false;
}

/**
 * Create global fieldname arrays for the following field types: MEMO, CHAR, FLOAT + INTEGER
 *
 * @param boolean $force - Optional parameter (default = false):  If true recreate all entries, else only if not already set
 */
function fxf_dbFields($force=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Field types array
	$fna=array(
		'memo'=>'3,15',
		'char'=>'1,8',
		'float'=>'6,11,16,17',
		'integer'=>'2,4,5,12,14'
	);

	// Create fieldname array for each type (char fields have their length value set here so we can check later if a text is short enough to fit into the table column)
	foreach($fna as $ftn => $fts)
	{
		if($force || !isset($GLOBALS['fxpglobals']['dbparam'][$ftn]) || !is_array($GLOBALS['fxpglobals']['dbparam'][$ftn]))
		{
			$GLOBALS['fxpglobals']['dbparam'][$ftn]=array();
			if($ftn == 'char')
				$sql="SELECT feldname,laenge FROM felder WHERE feldtyp IN (".$fts.") AND laenge IS NOT NULL AND laenge>0";
			else
				$sql="SELECT feldname FROM felder WHERE feldtyp IN (".$fts.")";
			$fa=db_values($sql." ORDER BY feldname", '*');
			if(fxIsArray($fa))
			{
				foreach($fa as $a)
				{
					if($ftn == 'char')
						$GLOBALS['fxpglobals']['dbparam'][$ftn][strtolower($a['feldname'])]=(int)$a['laenge'];
					else
						$GLOBALS['fxpglobals']['dbparam'][$ftn][strtolower($a['feldname'])]=true;
				}
			}
		}
	}

	// Create client tablename array, i.e. tables (except histoy tables) that have the column "Mandanten_ID" (field no. 22)
	if($force || !isset($GLOBALS['fxpglobals']['dbparam']['ctables']) || !is_array($GLOBALS['fxpglobals']['dbparam']['ctables']))
	{
		$GLOBALS['fxpglobals']['dbparam']['ctables']=array();
		$clienttables=db_values("SELECT t.tabname, t.tabelleart FROM tabellen t, tab_felder f WHERE (t.id_tabellen>0 OR t.id_tabellen<=-10000) AND f.idintabellen=t.id_tabellen AND f.idinfelder=22 ".db_orderby('t.tabname'), '*');
		if(fxIsArray($clienttables))
		{
			foreach($clienttables as $value)
			{
				$tabnam=strtolower($value['tabname']);
				$tabtype=(int)$value['tabelleart'];

				if(($tabnam == 'felder') || ($tabnam == 'masken'))	// Change these tables to type 4 = Individual
					$tabtype=4;

				$GLOBALS['fxpglobals']['dbparam']['ctables'][$tabnam]=$tabtype;
			}
		}
	}

	// Create table without "tarnsid" array, i.e. tables that don't have the column "transid" (field no. 12)
	if($force || !isset($GLOBALS['fxpglobals']['dbparam']['ntidtables']) || !is_array($GLOBALS['fxpglobals']['dbparam']['ntidtables']))
	{
		$GLOBALS['fxpglobals']['dbparam']['ntidtables']=array();
		$sql="SELECT tabname, tabelleart FROM tabellen WHERE id_tabellen NOT IN (SELECT idintabellen FROM tab_felder WHERE idinfelder=12)";
		$tables=db_values($sql, '*');
		if(fxIsArray($tables))
		{
			foreach($tables as $value)
			{
				$tabnam=strtolower($value['tabname']);
				$tabtype=(int)$value['tabelleart'];

				if(($tabnam == 'felder') || ($tabnam == 'masken'))	// Change these tables to type 4 = Individual
					$tabtype=4;

				$GLOBALS['fxpglobals']['dbparam']['ntidtables'][$tabnam]=$tabart;
			}
		}
	}
}

/**
 * Get current database, program and patch version
 */
function db_version()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Get database version
	$GLOBALS['fxpglobals']['dbparam']['dbvers']=array();
	// ...for PostgreSQL
	if($GLOBALS['_dbtype'] == 'pgsql')
	{
		$vsql="SELECT version()";
		$vtt=trim(db_value($vsql));
		if(strlen($vtt))
		{
			$vte=explode(' ', $vtt);
			$GLOBALS['fxpglobals']['dbparam']['dbvers']=array('vers'=>$vte[1], 'name'=>$vte[0], 'version'=>$vte[1]);
		}
	}
	// ...for SQLServer
	else if($GLOBALS['_dbtype'] == 'sqlsrv')
	{
		$vsql="SELECT CAST(SERVERPROPERTY('edition') AS VARCHAR) AS name, CAST(SERVERPROPERTY('productversion') AS VARCHAR) AS version, CAST(SERVERPROPERTY('productlevel') AS VARCHAR) AS level";
		$vtt=db_values($vsql);
		if(is_array($vtt))
		{
 			$iv=(int)$vtt['version'];
			if($iv > 13)
				$nz='2017';
			else if($iv > 12)
				$nz='2016';
			else if($iv > 11)
				$nz='2014';
			else if($iv > 10)
				$nz='2012';
			else if($iv > 9)
			{
 				if($vtt['version'] >= 10.25)
					$nz='2010';
 				else
					$nz='2008';
			}
			else if($iv > 8)
				$nz='2005';
			else if($iv > 7)
				$nz='2000';
			else
				$nz=$iv.'.0';
			$vv=$vtt['version'];
			if(strlen($vtt['level']))
				$vv .= ' - '.$vtt['level'];
			$GLOBALS['fxpglobals']['dbparam']['dbvers']=array('vers'=>$vtt['version'], 'name'=>'SQL Server '.$nz.' - '.$vtt['name'], 'version'=>$vv);
		}
	}
	if(sizeof($GLOBALS['fxpglobals']['dbparam']['dbvers']))
	{
		$vsp=explode('.', $GLOBALS['fxpglobals']['dbparam']['dbvers']['vers']);
		$nvf='';
		foreach($vsp as $vt)
			$nvf .= substr('000'.(int)$vt, -3);
		$GLOBALS['fxpglobals']['dbparam']['dbvers']['vers']=$nvf;
	}

	// Get current program version
	$versionscode='';
	$vex=explode('.', FXP_VERSION);
	if(sizeof($vex))
	{
		foreach($vex as $vcn)
			$versionscode .= substr('000'.$vcn, -3);
	}
	$GLOBALS['fxpglobals']['dbparam']['version_prg']=FXP_VERSION;
	$GLOBALS['fxpglobals']['dbparam']['versionscode_prg']=substr($versionscode.'000000000',0,9);

	// Set default database and patch version
	$GLOBALS['fxpglobals']['dbparam']['version_db']='999.999.999';
	$GLOBALS['fxpglobals']['dbparam']['versionscode_db']='999999999';
	$GLOBALS['fxpglobals']['dbparam']['version_patch']='0';

	$GLOBALS['fxpglobals']['dbparam']['date_version']='';
	$GLOBALS['fxpglobals']['dbparam']['date_patch']='';
}

/**
 * Set database parameters like "cascade", "backslash" or "cast"
 */
function db_param()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Use CASCADE (PostgreSQL Version 7.3.x +)
	$GLOBALS['fxpglobals']['dbparam']['dbcascade']="";
	if(($GLOBALS['_dbtype'] == 'pgsql') && (strncmp($GLOBALS['fxpglobals']['dbparam']['dbvers']['vers'], '007003', 6) >= 0))
		$GLOBALS['fxpglobals']['dbparam']['dbcascade']=" CASCADE";

	// Disable Backslash-Escape-Character in SQL's (PostgreSQL Version 8.2.x +)
	if(($GLOBALS['_dbtype'] == 'pgsql') && (strncmp($GLOBALS['fxpglobals']['dbparam']['dbvers']['vers'], '008002', 6) >= 0))
		@db_query("SET standard_conforming_strings=ON");

	// CAST Variable
	$GLOBALS['fxpglobals']['dbparam']['dbcast']="char";
}

/**
 * db_oeffnen
 * Oeffnet die zu verwendende Datenbank, gibt Handle auf dieselbe zurueck, oder
 * false im Fehlerfall
 * -----------------------------------------------------------------------------
 * Ergänzung:	Die notwendigen Parameter befinden sich in der Variable
 * $GLOBALS['fxpglobals']['dbparam'], welche mit der Funktion
 * db_ini erstellt	wird
 *
 * @param boolean $force - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_oeffnen($force=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['_dbtype']=$GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['type'];

	if($GLOBALS['dbid'] && !$force)
		return $GLOBALS['dbid'];

	if(!fxIsArray($GLOBALS['fxdb']) || !isset($GLOBALS['fxdb']['username']) || !strlen($GLOBALS['fxdb']['username']))
		fxf_dbGetCP();

	$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

	$message='db_oeffnen()';
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] == 1969)
		$message .= ' {'.$dbbtf.' ['.$dbbtl.']}';
	$message .= ': ';

	$fehler=0;

	switch($GLOBALS['_dbtype'])
	{
		case 'pgsql':
			// Verbindung mittels Host oder auf lokalem Server
			if(strlen($GLOBALS['fxpglobals']['dbparam']['dbserver']))
				$dbstr='host='.$GLOBALS['fxpglobals']['dbparam']['dbserver'].' ';
			else
				$dbstr='';
			$dbstr .= 'dbname='.$GLOBALS['fxpglobals']['dbparam']['dbname'].' user='.$GLOBALS['fxdb']['username'].' password='.$GLOBALS['fxdb']['password'];
			$message .= 'PG_CONNECT... ';
			if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
				db_protokoll('start', $message);
			$ergebnis=@pg_connect($dbstr);
			$enc_err=@pg_set_client_encoding($ergebnis, FXP_CHARSET);
			if(!$ergebnis || $enc_err)
				$fehler=__LINE__;
		break;

		case 'sqlsrv':
			// Verbindungsinfos werden in einem Array mitgegeben: Benutzername, Passwort und Datenbankname
			$connection_info=array('UID'=>$GLOBALS['fxdb']['username'], 'PWD'=>$GLOBALS['fxdb']['password'], 'Database'=>$GLOBALS['fxpglobals']['dbparam']['dbname'], 'CharacterSet'=>FXP_CHARSET);
			$message .= 'SQLSRV_CONNECT... ';
			if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
				db_protokoll('start', $message);
			$ergebnis=@sqlsrv_connect($GLOBALS['fxpglobals']['dbparam']['dbserver'], $connection_info);
			if(!$ergebnis)
				$fehler=__LINE__;
		break;

		default:
			$fehler=__LINE__;
	}

	// Protokoll
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 0)
	{
		$mode='write';
		if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
		{
			$mode='commit';
			$message='';
		}

		if($fehler)
		{
			$message .= 'ERROR '.$fehler.' {'.$dbbtf.' ['.$dbbtl.']} - Kann keine Datenbank-Verbindung öffnen?';
			$message .= '  [Einstellungen: dbserver="'.$GLOBALS['fxpglobals']['dbparam']['dbserver'].'", dbname="'.$GLOBALS['fxpglobals']['dbparam']['dbname'].'"]';
			db_protokoll($mode, $message);
		}
		else if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
		{
			$message .= 'OK. - Datenbank-Verbindung wurde geöffnet.';
			db_protokoll($mode, $message);
		}
	}

	// Debugmeldung
	if($GLOBALS['debug_db'] && $fehler)
		echo('<div style="padding:8px;margin:0 1px;border-radius:4px;background:#e1001a;color:#000000;"><b>ERROR:</b><br />'.$message.'</div>'.$GLOBALS['nl']);

	return $ergebnis;
}

/**
 * db_schliessen
 * Schliesst die mit db_oeffnen aufgemachte Datenbank, wird angeblich beim
 * Beenden eines PHP-Scripts automatisch gemacht (aber sicher ist sicher)...
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 */
function db_schliessen()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Keine aktive Datenbankverbindung?
	if(!$GLOBALS['dbid'])
		return;

	$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

	$message='db_schliessen()';
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] == 1969)
		$message .= ' {'.$dbbtf.' ['.$dbbtl.']}';
	$message .= ': ';

	$fehler=0;

	switch($GLOBALS['_dbtype'])
	{
		case 'pgsql':
			$message .= 'PG_CLOSE... ';
			if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
				db_protokoll('start', $message);
			$ergebnis=@pg_close($GLOBALS['dbid']);
			if($ergebnis == false)
				$fehler=__LINE__;
		break;

		case 'sqlsrv':
			$message .= 'SQLSRV_CLOSE... ';
			if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
				db_protokoll('start', $message);
			$ergebnis=@sqlsrv_close($GLOBALS['dbid']);
			if(!$ergebnis)
				$fehler=__LINE__;
		break;

		default:
			$fehler=__LINE__;
	}

	// Protokoll
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 0)
	{
		$mode='write';
		if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
		{
			$mode='commit';
			$message='';
		}

		if($fehler)
		{
			$message .= 'ERROR '.$fehler.' {'.$dbbtf.' ['.$dbbtl.']} - Kann Datenbank-Verbindung nicht schliessen?';
			db_protokoll($mode, $message);
		}
		else if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
		{
			$message .= 'OK. - Datenbank-Verbindung wurde geschlossen.';
			db_protokoll($mode, $message);
		}
	}

	// Debugmeldung
	if($GLOBALS['debug_db'] && $fehler)
		echo('<div style="padding:8px;margin:0 1px;border-radius:4px;background:#e1001a;color:#000000;"><b>ERROR:</b><br />'.$message.'</div>'.$GLOBALS['nl']);

	$GLOBALS['dbid']=false;
}

/**
 * db_query
 * Führt einen SQL-Befehl auf der DB aus. Falls DB nicht geöffnet wird dies
 * ebenfalls vorher ausgeführt.
 * -----------------------------------------------------------------------------
 * Rückgabe: Das Handle auf das Ergebnis der Abfrage, oder false im Fehlerfall
 *
 * @param various $sql   - Mandatory parameter:                ???PARAMETER???
 * @param integer $dbbtl - Optional parameter (default = 0):   ???PARAMETER???
 * @param string  $dbbtf - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_query($sql, $dbbtl=0, $dbbtf='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql=trim($sql);
	if(!strlen($sql))	// Kein SQL übergeben, dann gleich Fehler zurückliefern
		return false;

	if(!strlen($dbbtf))
	{
		$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];
	}

	// SQL-Befehle vorbereiten
	$arr=db_sql2array($sql);
	if(fxIsArray($arr))
	{
		if($GLOBALS['_maxlength'] && (($arr['mode'] == 'ins') || ($arr['mode'] == 'upd')))
			return false;

		// Bei Mandanten-Tabellen prüfen, ob Mandanten-ID mitgegeben wurde in der WHERE-Bedingung - falls nicht, dann setzen
		$sql=db_sqlAddClient($sql, $arr, $dbbtl, $dbbtf);
	}

	// Datenbankverbindung herstellen, falls noch nicht vorhanden
	if(!$GLOBALS['dbid'])
	{
		$GLOBALS['dbid']=@db_oeffnen();
		if(!$GLOBALS['dbid'])
			return false;
	}

	$message='db_query("'.$sql.'")';
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] == 1969)
		$message .= ' {'.$dbbtf.' ['.$dbbtl.']}';
	$message .= ': ';

	if(isset($GLOBALS['swritelog']) && strlen($GLOBALS['swritelog']))
		db_slog($sql.'; ... ', $dbbtl, $dbbtf, 'start');

	$fehler=0;
	$GLOBALS['_dberrmsg']='';

	switch($GLOBALS['_dbtype'])
	{
		case 'pgsql':
			$message .= 'PG_QUERY... ';
			if(($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1) && (!isset($GLOBALS['db_noerror']) || !$GLOBALS['db_noerror']))
				db_protokoll('start', $message);
			if(isset($GLOBALS['db_noerror']) && $GLOBALS['db_noerror'])
				$ergebnis=@pg_query($GLOBALS['dbid'], $sql);
			else
				$ergebnis=pg_query($GLOBALS['dbid'], $sql);
			if($ergebnis == false)
			{
				$fehler=__LINE__;
				$GLOBALS['_dberrmsg']=pg_last_error($GLOBALS['dbid']); 
			}
		break;

		case 'sqlsrv':
			$message .= 'SQLSRV_QUERY... ';
			if(($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1) && (!isset($GLOBALS['db_noerror']) || !$GLOBALS['db_noerror']))
				db_protokoll('start', $message);
			if(isset($GLOBALS['sqlsrv_param_out']))
				unset($GLOBALS['sqlsrv_param_out']);
			// Ausnahme für Stored Procedures
			$params=false;
			$f5=strtoupper(substr(trim($sql),0,5));
			if($f5 == '{CALL')
			{
				$val=0;
				$params=array(
					array(&$val, SQLSRV_PARAM_INOUT)
				);

				$ergebnis=@sqlsrv_query($GLOBALS['dbid'], $sql, $params);
				if($ergebnis)
				{
					sqlsrv_next_result($ergebnis);
					$GLOBALS['sqlsrv_param_out']=$val;
//echo('<b>sqlsrv_param_out</b>='.$GLOBALS['sqlsrv_param_out'].'<br />');
				}
			}
			else if(isset($GLOBALS['db_noerror']) && $GLOBALS['db_noerror'])
				$ergebnis=@sqlsrv_query($GLOBALS['dbid'], $sql);
			else
				$ergebnis=sqlsrv_query($GLOBALS['dbid'], $sql);
			if(!$ergebnis)
			{
				// Ausnahme: Bei Tabellenumbenennungen mit sp_rename wird die Warnmeldung "Caution: Changing any part of an object name could break scripts and stored procedures" produziert, welche ignoriert werden kann/muss.
				$sprp=stripos($sql, 'sp_rename ');
				if($sprp !== false)
					$ergebnis=true;
				else
				{
					$fehler=__LINE__;
					$err_array=sqlsrv_errors();
//fxDebug($err_array, '$err_array');
					if(fxIsArray($err_array))
					{
						foreach($err_array as $error)
						{
							if(strlen($GLOBALS['_dberrmsg']))
								$GLOBALS['_dberrmsg'] .= '; ';
							$GLOBALS['_dberrmsg'] .= 'SQLSTATE: '.$error['SQLSTATE'].' -- code: '.$error['code'].' -- message: '.$error['message'];
						}
					}
				}
			}
		break;

		default:
			$fehler=__LINE__;
 	}

	// Fehler bei Entwicklung immer anzeigen
	$send_error=true;
	if($fehler && !$GLOBALS['db_noerror'])
	{
		if($fehler)
			echo('<b class="red">Fehler in Zeile '.$dbbtl.', Datei "'.$dbbtf.'":</b><br /><font class="red">'.$GLOBALS['_dberrmsg'].'</font><br /><i>['.$sql.';]</i><hr />'.$GLOBALS['nl']);
		else
			echo('<i>['.$sql.';]</i><hr />'.$GLOBALS['nl']);

		$send_error=false;
	}

	if(isset($GLOBALS['swritelog']) && strlen($GLOBALS['swritelog']))
	{
		if($fehler)
		{
			$sqlstat='ERROR!';
			if(strlen($GLOBALS['_dberrmsg']))
				$sqlstat .= ' ('.$GLOBALS['_dberrmsg'].')';
		}
		else
			$sqlstat='OK.';

		db_slog('['.$sqlstat.']', $dbbtl, $dbbtf, 'end');
	}
//echo(fxHtmlEncode($sql).'<br />');

	// Protokoll
	if(($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 0) && (!isset($GLOBALS['db_noerror']) || !$GLOBALS['db_noerror']))
	{
		$mode='write';
		if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
		{
			$mode='commit';
			$message='';
		}

		if($fehler)
		{
			$message .= 'ERROR '.$fehler.' {'.$dbbtf.' ['.$dbbtl.']} - Kann SQL-Befehl nicht ausführen?';
			if(strlen($GLOBALS['_dberrmsg']))
				$message .= ' DB-Fehlermeldung: "'.$GLOBALS['_dberrmsg'].'"?';
			db_protokoll($mode, $message);
		}
		else if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
		{
			$message .= 'OK. - SQL-Befehl wurde erfolgreich ausgeführt.';
			db_protokoll($mode, $message);
		}
	}

	return $ergebnis;
}

/**
 * db_x
 * Führt die Datenbankzugriffe aus. Dazu müssen das eigentliche SQL-Statement
 * und eine Prüfabfrage (ob der zu bearbeitende Datensatz vorhanden ist oder
 * nicht) als String übergeben werden.
 * Zurückgegeben wird der Fehlerstatus (true oder false: Fehlerkein Fehler).
 * Bei Schreibzugriffen werden AenderungsID und Zeitstempel automatisch ein-
 * gefügtaktualisiert.
 * -----------------------------------------------------------------------------
 * Übergabeparameter:
 * °°°°°°°°°°°°°°°°°°
 * $sql:        SQL-Statement, das ausgeführt werden soll
 * (SELECT, INSERT, UPDATE oder DELETE)
 * $meldung:	(optional) 0, d.h. keine Fehlermeldungen anzeigen
 * $debug:      (optional) true, so werden Debugmeldungen angezeigt
 * -----------------------------------------------------------------------------
 * Autor: Kurt
 *
 * @param various $sql             - Mandatory parameter:                   ???PARAMETER???
 * @param integer $meldung         - Optional parameter (default = 0):      ???PARAMETER???
 * @param boolean $debug           - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $change_aendzeit - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $make_history    - Optional parameter (default = true):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_x($sql, $meldung=0, $debug=false, $change_aendzeit=true, $make_history=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

	$_tr=(int)$GLOBALS['fxpglobals']['tr'];
	$jetzt=fxNow();
	$_hts=$GLOBALS['_history_timestamp'];
	if(!strlen($_hts) || (strlen($_hts) != 14))
		$_hts=$jetzt;

	$fehler=0;
	$mld_ok=0;
	$historie=0;

	if((gettype($meldung) == 'integer') && ($meldung == 0))
		$showerror=false;
	else
		$showerror=true;

	// Bestimmen der SQL-Abfrageart
	$sql=trim($sql);
	$sqll=strtolower($sql);
	$sql_art=substr($sqll, 0, 3);

	$tabname='';
	switch($sql_art)
	{
		case 'ins':
			$etp=strpos($sqll, '(', 12);
			if($etp)
				$tabname=trim(substr($sqll, 12, $etp-12));

			$pos=db_subpos($sqll, ')');
			$coladd='';
			$valadd='';
			// Bei Maskengeneratormasken automatisch die Spalte Mandanten-ID ergänzen, ausser diese ist bereits vorhanden
			if(($_tr < 0) && !strpos($sqll, 'mandanten_id'))
			{
				$coladd .= ", mandanten_id";
				$valadd .= ", ".$GLOBALS['fxpglobals']['client'];
			}
			if($make_history && is_array($GLOBALS['fxpglobals']['historytables']) && is_array($GLOBALS['fxpglobals']['historytables'][$tabname]))
			{
				$coladd .= ", historie";
				$valadd .= ", 1";

				$historie=2;
			}
			if(!isset($GLOBALS['fxpglobals']['dbparam']['ntidtables'][$tabname]))
			{
				$coladd .= ", transid";
				$valadd .= ", ".$_tr;
			}
			$sql=substr($sql, 0, $pos).$coladd.", aenderungs_id, zeitstempel".substr($sql, $pos, strlen($sql)-$pos-1).$valadd.", ".$GLOBALS['fxpglobals']['person'].", '".$jetzt."')";

			// 457: Der Datensatz wurde erfolgreich angelegt.
			$mld_ok=457;
		break;

		case 'upd':
			$etp=strpos($sqll, ' ', 7);
			if($etp)
				$tabname=trim(substr($sqll, 7, $etp-7));

			$pos=db_subpos($sqll, ' where ');
			$updadd='';
			if($make_history && is_array($GLOBALS['fxpglobals']['historytables']) && is_array($GLOBALS['fxpglobals']['historytables'][$tabname]))
			{
				$updadd .= ", historie=1";

				$historie=3;
			}
			if($change_aendzeit)
			{
				if(!isset($GLOBALS['fxpglobals']['dbparam']['ntidtables'][$tabname]))
					$updadd .= ", transid=".$_tr;
				$updadd .= ", aenderungs_id=".$GLOBALS['fxpglobals']['person'].", zeitstempel='".$jetzt."'";
			}
			if(strlen($updadd))
				$sql=substr($sql, 0, $pos).$updadd.substr($sql, $pos);

			// 459: Der Datensatz wurde erfolgreich geändert.
			$mld_ok=459;
		break;

		case 'del':
			$etp=strpos($sqll, ' ', 12);
			if($etp)
				$tabname=trim(substr($sqll, 12, $etp-12));

			if($make_history && is_array($GLOBALS['fxpglobals']['historytables']) && is_array($GLOBALS['fxpglobals']['historytables'][$tabname]))
			{
				$pos=db_subpos($sqll, ' where ');
//echo('<PRE>'.fxHtmlEncode($sql).'</PRE>');
				$hsql="INSERT INTO ".$tabname."_history (trans_akt_id, aktion".$GLOBALS['fxpglobals']['historytables'][$tabname]['fld'];
				if(!isset($GLOBALS['fxpglobals']['dbparam']['ntidtables'][$tabname.'_history']))
					$hsql .= ", transid";
				$hsql .= ", aenderungs_id, zeitstempel) SELECT ".$_tr." AS trans_akt_id, 4 AS aktion".$GLOBALS['fxpglobals']['historytables'][$tabname]['fld'];
				if(!isset($GLOBALS['fxpglobals']['dbparam']['ntidtables'][$tabname.'_history']))
					$hsql .= ", ".(int)$GLOBALS['fxpglobals']['tr']." AS transid";
				$hsql .= ", ".$GLOBALS['fxpglobals']['person']." AS aenderungs_id, '".$_hts."' AS zeitstempel FROM ".$tabname.substr($sql, $pos);
				if(isset($GLOBALS['fxpglobals']['dbparam']['ctables'][$tabname]) && $GLOBALS['fxpglobals']['client'])
					$hsql .= " AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$success=db_query($hsql, $dbbtl,$dbbtf);
//fxDebug($success, $hsql);
				if(!$success)
					$fehler=__LINE__;
			}

 			// 461: Der Datensatz wurde erfolgreich gelöscht.
 			$mld_ok=461;
		break;
	}

	// SQLs in Datei schreiben
	if($_GET['sqldebug'])
		fxSave($GLOBALS['logpath'].'~~debug.log', $dbbtf.' ['.$dbbtl.']'.$GLOBALS['nl_b'].$sql.$GLOBALS['nl_b'].$GLOBALS['nl_b'], false);

	$esql=$sql;
	$ergebnis=db_query($sql, $dbbtl,$dbbtf);	// SQL ausführen
//fxDebug($ergebnis, $sql);
	if($ergebnis == false)
		$fehler=__LINE__;

	// Datensätze in Historie schreiben
	if(!$fehler && $make_history && $historie && strlen($tabname))
	{
//echo('<PRE>'.fxHtmlEncode($sql).'</PRE>');
		$hsql="INSERT INTO ".$tabname."_history (trans_akt_id, aktion".$GLOBALS['fxpglobals']['historytables'][$tabname]['fld'];
		if(!isset($GLOBALS['fxpglobals']['dbparam']['ntidtables'][$tabname.'_history']))
			$hsql .= ", transid";
		$hsql .= ", aenderungs_id, zeitstempel) SELECT ".$_tr." AS trans_akt_id, ".$historie." AS aktion".$GLOBALS['fxpglobals']['historytables'][$tabname]['fld'];
		if(!isset($GLOBALS['fxpglobals']['dbparam']['ntidtables'][$tabname.'_history']))
			$hsql .= ", transid";
		$hsql .= ", aenderungs_id, ".$_hts." AS zeitstempel FROM ".$tabname." WHERE historie=1";
		if(isset($GLOBALS['fxpglobals']['dbparam']['ctables'][$tabname]) && $GLOBALS['fxpglobals']['client'])
			$hsql .= " AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$esql=$hsql;
		$success=db_query($hsql, $dbbtl,$dbbtf);
//fxDebug($success, $hsql);
		if($success)
		{
			$dsql="UPDATE ".$tabname." SET historie=0 WHERE historie=1";
			if(isset($GLOBALS['fxpglobals']['dbparam']['ctables'][$tabname]) && $GLOBALS['fxpglobals']['client'])
				$dsql .= " AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$esql=$dsql;
			$success=db_query($dsql, $dbbtl,$dbbtf);
//fxDebug($success, $dsql);
		}
		if(!$success)
			$fehler=__LINE__;
	}

	// Debugmeldung
	if($fehler && !$GLOBALS['_maxlength'] && $GLOBALS['debug_db'])
		echo('<b class="red">'.$fehler.'</b><hr />'.$sql.'<hr />'.$GLOBALS['nl']);

	// Rückgabe des Fehlerstatus: true=mit Fehler, false=ohne Fehler
	if($fehler)
		return true;

	if($meldung && $mld_ok) // Datenbankzugriff war erfolgreich
		error_msg($mld_ok, false);

	return false;
}

/**
 * db_value
 * Gibt einen Feldwert aus dem aktuellen Datensatz des SQL-Ergebnisses zurück.
 * Rueckgabe: der Feldwert oder false im Fehlerfall
 *
 * @param various $sql      - Mandatory parameter:                ???PARAMETER???
 * @param string  $feldname - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_value($sql, $feldname='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

	$feldname=strtolower($feldname);
	$message='db_value([query], "'.$feldname.'") ['.$GLOBALS['_dbtype'].']';
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] == 1969)
		$message .= ' {'.$dbbtf.' ['.$dbbtl.']}';
	$message .= ': ';

	// SQL
	$sql_id=db_query($sql, $dbbtl,$dbbtf);

	if($sql_id !== false)
	{
		switch($GLOBALS['_dbtype'])
		{
			case 'pgsql':
				$message .= 'PG_FETCH_ARRAY... ';
				if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
					db_protokoll('start', $message);
				$ergebnis='';

				$array=@pg_fetch_array($sql_id, 0, PGSQL_ASSOC);
				if(is_array($array))
				{
					foreach($array as $key => $value)
					{
						$lfeldname=strtolower($key);
					    if(!strlen($feldname) || ($feldname == $lfeldname))
					    {
							$ergebnis=db_convert_column($lfeldname, $value);
							break;
						}
				    }
				}
			break;

			case 'sqlsrv':
				if(isset($GLOBALS['sqlsrv_param_out']))
				{
					$ergebnis=$GLOBALS['sqlsrv_param_out'];
					unset($GLOBALS['sqlsrv_param_out']);
				}
				else
				{
					$message .= 'SQLSRV_FETCH_ARRAY... ';
					if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
						db_protokoll('start', $message);
					$ergebnis='';

					$array=@sqlsrv_fetch_array($sql_id, SQLSRV_FETCH_ASSOC);
					if(fxIsArray($array))
					{
						foreach($array as $key => $value)
						{
							$lfeldname=strtolower($key);
						    if(!strlen($feldname) || ($feldname == $lfeldname))
						    {
								$ergebnis=db_convert_column($lfeldname, $value);
								break;
							}
					    }
					}
				}
			break;
		}
	}
	else
	{
		$message .= '<FALSE>';
		if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
			db_protokoll('start', $message);
		$ergebnis=false;
	}

	if($GLOBALS['debug_db'] > 1)
		echo('<b><FONT COLOR="#0000AA">DB_WERT:</FONT></b><br />$feldname="'.$feldname.'" -> "'.$ergebnis.'"<HR NOSHADE SIZE="1">');

	// Protokoll
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
	{
		$mode='commit';
		$message='OK. - Das Feld ';

		if(strlen($feldname))
			$message .= '['.$feldname.'] ';
		$message .= 'enthält den Wert "'.$ergebnis.'".';
		db_protokoll($mode, $message);
	}

	return $ergebnis;
}

/**
 * db_err
 * gibt letzten DB-Fehler zurück
 *
 * @return ???RETURN???
 */
function db_err()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$fehler='';

	switch($GLOBALS['_dbtype'])
	{
		case 'pgsql':
			if($GLOBALS['dbid'])
				$fehler=pg_last_error($GLOBALS['dbid']);
		break;

		case 'sqlsrv':
			$err_array=sqlsrv_errors();
			if(fxIsArray($err_array))
			{
				foreach($err_array as $error)
				{
					if(strlen($fehler))
						$fehler .= '<br />'.$GLOBALS['nl'];
					$fehler .= 'SQLSTATE: '.$error['SQLSTATE'].' -- code: '.$error['code'].' -- message: '.$error['message'];
				}
			}
		break;
 	}

	if(!strlen($fehler))
		$fehler='[Unknown]';

	return $fehler;
}

/**
 * setzt im zurückzugebenden Array die Werte und die Indizes
 * @param array $return_array das zurückzugebenden Array, als Referenz
 * @param mixed $key der Schlüssel des aktuellen Feldes
 * @param mixed $value der Wert des aktuellen Feldes
 * @param mixed $entries der aktuelle Zeilencounter, '' wenn nur eine Zeile geholt wurde
 * @param array $unique, falls gesetzt, dann sind das die Schlüssel anhand derer indiziert wird
 */
function db_unique_keys(&$return_array, $key, $value, $entries, $unique)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Erstes Feld und zweites Feld sind Schlüssel: array[id_1][id_2]['feldname']
	// bei mode=-2 werden Einträge für 'id_1' und 'id_2' nicht ins Array geschrieben
	if(is_array($unique[1]))
	{
		if($unique['mode'] == 2 || ($key != $unique[0]['key'] && $key != $unique[1]['key']))
		{
			if($unique['no_key'])
				$return_array[$unique[0]['value']][$unique[1]['value']]=$value;
			else
				$return_array[$unique[0]['value']][$unique[1]['value']][$key]=$value;
		}
	}
	// Erstes Feld ist Schlüssel: array[id_1]['feldname']
	// bei mode=-1 wird Eintrag für 'id_1' nicht ins Array geschrieben
	else if(is_array($unique[0]))
	{
		if($unique['mode'] == 1 || $key != $unique[0]['key'])
		{
			if($unique['no_key'])
				$return_array[$unique[0]['value']]=$value;
			else
				$return_array[$unique[0]['value']][$key]=$value;
		}
	}
	// wie bisher ...
	else
	{
		// ... array['feldname'] wenn nur eine Zeile zurückgegeben werden soll ...
		if(strlen($entries) == 0)
			$return_array[$key]=$value;
		// ... bzw. array[counter]['feldname'] wenn mehrere Zeilen zurückgegeben werden soll
		else
			$return_array[$entries][$key]=$value;
	}
}

/**
 * liefert aus einer SQL-Ressource die nächste Zeile als assoziatives Array
 * @param resource $sql_id die Ressource bei der der Pointer gesetzt werden soll
 * @param string $db_typ der Datenbank-Typ (pgsql, sqlsrv)
 * @return array das Array mit den Daten der nächsten Zeile
 */
function db_get_array($sql_id, $db_typ=0, $tolower=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	switch($GLOBALS['_dbtype'])
	{
		case 'pgsql':
			$ret_arr=pg_fetch_array($sql_id, null, PGSQL_ASSOC);
		break;

		case 'sqlsrv':
			$ret_arr=sqlsrv_fetch_array($sql_id, SQLSRV_FETCH_ASSOC);
		break;
	}

	if(is_array($ret_arr) && sizeof($ret_arr))
	{
		if($tolower)
		{
			$ret_arr_low=array();
			foreach($ret_arr as $key => $value)
			{
				$lfeldname=strtolower($key);
				$value=db_convert_column($lfeldname, $value);

				$ret_arr_low[$lfeldname]=$value;
			}

			return $ret_arr_low;
		}
		else
			return $ret_arr;
	}

	return false;
}

/**
 * setzt den Pointer in einer SQL-Ressource
 *
 * @param resource $sql_id die Ressource bei der der Pointer gesetzt werden soll
 * @param int $pos die Position auf die der Pointer vorrücken soll (beginnt bei 0)
 *
 * @return boolean true, wenn das Setzen erfolgreich war, false sonst
 */
function db_seek($sql_id, $pos)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	switch($GLOBALS['_dbtype'])
	{
		case 'pgsql':
			return @pg_result_seek($sql_id, $pos);
		break;

		case 'sqlsrv':
			$fp=@sqlsrv_fetch($sql_id);
			if($fp === false)
				return false;
			for($p=1; $p<$pos; $p++)
			{
				$fp=sqlsrv_fetch($sql_id, SQLSRV_SCROLL_NEXT);
				if($fp === false)
					return false;
				else if($fp !== true)
					break;
			}
			return true;
		break;
	}

	return false;
}

/**
 * db_values
 * Gibt alle Felder des aktuellen Datensatzes in ein Array aus, die Schlüssel-
 * felder des Arrays sind die Feldnamen (IN KLEINBUCHSTABEN!) des SQL, wenn
 * keine Datensätze (mehr) vorhanden sind, wird false anstatt des Arrays aus-
 * gegeben, anschließend wird automatisch zum nächsten Datensatz gewechselt.
 * -----------------------------------------------------------------------------
 * Erweiterte Funktion:
 * °°°°°°°°°°°°°°°°°°°°
 * mit	db_values("sql", ''); werden alle Datensätze zurückgeliefert in einem
 * Array mit dem Format $array[Zeilennummer][Feldname]
 * mit	db_values("sql", 10, 15); werden die Datensätze von Zeilennummer 10-15
 * (Zeilennummerierung ab 1 entspr. Datenbank) zurückgeliefert mit dem
 * Format $array[Zeilennummer][Feldname]
 * -----------------------------------------------------------------------------
 *
 * @param various $sql        - Mandatory parameter:                ???PARAMETER???
 * @param various $start      - Optional parameter (default = -1):  ???PARAMETER???
 * @param various $end        - Optional parameter (default = -1):  ???PARAMETER???
 * @param integer $uniqueKeys - Optional parameter (default = 0):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_values($sql, $start=-1, $end=-1, $uniqueKeys=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];
	$message='Funktion db_values(): '.$GLOBALS['_dbtype'].' ';

	// SQL
	$sql_id=db_query($sql, $dbbtl,$dbbtf);

	$message='db_values([query], '.$start.', '.$end.') ['.$GLOBALS['_dbtype'].']';
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] == 1969)
		$message .= ' {'.$dbbtf.' ['.$dbbtl.']}';
	$message .= ': ';

	// Array
	if($sql_id === false)
	{
		$ergebnis=false;
		$message .= '<FALSE>';
	}
	else
	{
		$ergebnis=array();
		$get_all=false;

		// Was für Datensätze ermitteln...
		// ...nur ein Datensatz, oder
		if(($start == -1) && ($end == -1))
		{
			$entries='';
			$message .= '<einzelne Zeile> ';
		}
		// ...alle Datensätze, oder
		else if($start == '*')
		{
			$entries=0;
			$message .= '<alle Einträge> ';
		}
		// ...einen Bereich
		else
		{
			$entries=$start;
			$message .= '<Bereich von '.$start.' bis '.$end.'> ';
		}

		// Internen Pointer setzen
		$seek=true;
		if($start > 0)
			$seek=db_seek($sql_id, max(0, $start));
		if($seek !== false)
		{
			do
			{
				// Array ab dieser Position extrahieren
				$result_array=db_get_array($sql_id);
				if(is_array($result_array))
				{
					// Festlegen, ob und wie das Array indiziert werden soll
					$unique=array();
					if(abs($uniqueKeys) >= 1)
					{
						$uk=key($result_array);
						$uv=current($result_array);
						$unique[0]=array(0=>$uk, 'key'=>$uk, 1=>$uv, 'value'=>$uv);
					}
					if(abs($uniqueKeys) == 2)
					{
						$uk=key($result_array);
						$uv=current($result_array);
						$unique[1]=array(0=>$uk, 'key'=>$uk, 1=>$uv, 'value'=>$uv);
					}
					$unique['mode']=$uniqueKeys;
					// Anzahl der selektierten Felder genau eins höher als Anzahl der Keys
					// => zusätzlichen Key weglassen
					$unique['no_key']=(abs($uniqueKeys) + 1 == count($result_array));
					// Resultat in Rückgabe-Array schreiben ...
					foreach($result_array as $key => $value)
						db_unique_keys($ergebnis, $key, $value, $entries, $unique);
				}
				else if((int)$entries == 0)
				{
					$message .= 'Keine Einträge gefunden!';
					$ergebnis=false;
				}
				$entries++;
			}
			// Solange bis Resultat leer ist oder alle gewünschten Zeilen geholt wurden
			while(is_array($result_array) && ((++$start <= $end) || ($start === '*')))
				;

			$message .= $entries.' Einträge gefunden.';
		}
		else
		{
			$ergebnis=false;
			$message .= 'db_seek('.max(0, $start).') lieferte ungültige Position!';
		}
	}

	// Protokoll
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
		db_protokoll('write', $message);

	return $ergebnis;
}

/**
 * Begin, Commit or Rollback database transaction
 * - BEGIN     sets a transaction start marker
 * - COMMIT    performs all database actions from the start marker to now
 * - ROLLBACK  reverses all database actions upto the start marker
 *
 * @param string $act - Optional parameter (default = 's'):  Database action, like 'b' or 's'=[B]EGIN database transaction ([S]TART), 'c' or 'o'=[C]OMMIT transaction ([O]K) and 'r' or 'e'=[R]OLLBACK transaction ([E]RROR)
 */
function db_rollback($act='b')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$act=substr(strtolower(trim($act)),0,1);

	switch($GLOBALS['_dbtype'])
	{
		case 'pgsql':
			if(($act == 'b') || ($act == 's'))		// Begin transaction -> Set transaction start marker
			{
				$sql='BEGIN';
				$GLOBALS['db_in_trans']=true;
			}
			else
			{
				if(($act == 'c') || ($act == 'o'))	// Commit database transaction
					$sql='COMMIT';
				else								// Rollback database transaction
					$sql='ROLLBACK';
				$GLOBALS['db_in_trans']=false;
			}

			@pg_query($GLOBALS['dbid'],  $sql);
		break;

		case 'sqlsrv':
			if(($act == 'b') || ($act == 's'))		// Begin transaction -> Set transaction start marker
			{
				$ergebnis=sqlsrv_begin_transaction($GLOBALS['dbid']);
				$GLOBALS['db_in_trans']=true;
			}
			else
			{
				if(($act == 'c') || ($act == 'o'))	// Commit database transaction
					$ergebnis=sqlsrv_commit($GLOBALS['dbid']);
				else								// Rollback database transaction
					$ergebnis=sqlsrv_rollback($GLOBALS['dbid']);
				$GLOBALS['db_in_trans']=false;
			}
		break;
	}

	// Rollback
	if(($db_aktion != 'start') && ($db_aktion != 'ok'))
	{
		// Reseed max/min values that where determined by sequences
		if(fxIsArray($GLOBALS['db_mia']))
		{
//fxDebug($GLOBALS['db_mia'],'$GLOBALS[\'db_mia\']', 0);
			foreach($GLOBALS['db_mia'] as $seqname => $value)
				$success=fxf_dbIdSeqReseed($seqname, $value);
		}

		// Display message?
		if($GLOBALS['show_rollback_error_msg'])
			error_msg(2132, false);	// An error occurred while accessing the data base. The previous changes have been reversed.
	}

	$GLOBALS['db_mia']=array();
}

/**
 * ???FUNCTION???
 *
 * @param boolean $just_table - Optional parameter (default = true): ???PARAMETER???
 */
function dbGetColumns($just_table=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(isset($GLOBALS['_dbca']))
		return;

	$GLOBALS['_dbca']=array();

	// Check, if table "felder" exists
	$ftable_exists=db_check('felder','t');
	if(!$ftable_exists)
		return;
	// Check, if table "tab_felder" exists
	$tftable_exists=db_check('tab_felder','t');

	if($tftable_exists && $just_table)
		$sql="SELECT f.id,f.feldname,f.feldtyp,f.laenge,f.nachkomma FROM felder f, tab_felder t WHERE f.id=t.idinfelder AND f.mandanten_id IS NOT NULL";
	else
		$sql="SELECT id,feldname,feldtyp,laenge,nachkomma FROM felder WHERE mandanten_id IS NOT NULL";
	$fta=db_values($sql,'*');
//fxDebug($fta,$sql);
	if(fxIsArray($fta))
	{
		foreach($fta as $fa)
		{
			$fn=strtolower($fa['feldname']);
			$ft=(int)$fa['feldtyp'];
			if($ft == 2)														// Serial: 2=Auto Increment
				$tc='S';
			else if(($ft == 3) || ($ft == 15))									// Text: 3=Memo + 15=Text 4000 Characters
				$tc='T';
			else if($ft == 4)													// Small Integer: 4=Yes/No
				$tc='I2';
			else if(($ft == 5) || ($ft == 12) || ($ft == 14))					// Long Integer: 5=Integer, 12=Long Integer + 14=Timespan
				$tc='I8';
			else if(($ft == 6) || ($ft == 11) || ($ft == 16) || ($ft == 17))	// Decimal: 6=Decimal, 11=Amount, 16=Budget (Int) + 17=Budget (Ext)
				$tc='D22,4';
			else if(($ft == 7) || ($ft == 9) || ($ft == 10))					// Varchar 14: 7=Date, 9=Time + 10=Datetime
				$tc='C14';
			else																// Varchar: 1=Text, 8=Userformat + 13=Systemvar
			{
				$cl=(int)$fa['laenge'];
				if(!$cl)
					$cl=50;
				$tc='C'.$cl;
			}
			$GLOBALS['_dbca'][$fn]=array('id'=>(int)$fa['id'], 'itype'=>$ft, 'ftype'=>$tc);
		}
	}
//fxDebug($GLOBALS['_dbca'], '_dbca');
}

/**
 * ???FUNCTION???
 *
 * @param various $type - Mandatory parameter:                ???PARAMETER???
 * @param string  $op   - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $cp   - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbColumnType($type, $op='', $cp='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ft=substr($type,0,1);
	if($ft == 'S')			// Serial: Auto Increment
	{
		if($GLOBALS['_dbtype'] == 'sqlsrv')
			$tc="INT IDENTITY(1,1)";
		else
			$tc="SERIAL";
	}
	else if($ft == 'T')		// Text
		$tc="TEXT";
	else if($ft == 'I')		// Integer
	{
		if($GLOBALS['_dbtype'] == 'sqlsrv')
			$tc="INT";
		else
		{
			$cl=(int)substr($type,1);
			if(!$cl)
				$cl=8;
			$tc="INT".$cl;
		}
	}
	else if($ft == 'D')		// Decimal
		$tc="DECIMAL(22,4)";
	else					// Varchar
	{
		$cl=(int)substr($type,1);
		if(!$cl)
			$cl=50;
		$tc="VARCHAR(".$cl.")";
	}

	return $tc;
}

/**
 * ???FUNCTION???
 *
 * @param various $tn - Mandatory parameter: ???PARAMETER???
 */
function dbAddTableTypes($tn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(fxIsArray($GLOBALS['_dbta']) && fxIsArray($GLOBALS['_dbta'][$tn]) && fxIsArray($GLOBALS['_dbta'][$tn]['columns']))
	{
		foreach($GLOBALS['_dbta'][$tn]['columns'] as $cname => $ca)
		{
			$mcfi='';
			$val=true;

			$type=substr($ca['type'],0,1);
			switch($type)
			{
				case 'T':
					$mcfi='memo';
				break;

				case 'C':
					$mcfi='char';
					$val=(int)substr($ca['type'],1);
					if(!$val)
						$val=50;
				break;

				case 'D':
					$mcfi='float';
				break;

				default:
					$mcfi='integer';
				break;
			}

			if(!isset($GLOBALS['fxpglobals']['dbparam'][$mcfi]))
				$GLOBALS['fxpglobals']['dbparam'][$mcfi]=array();
			if(!isset($GLOBALS['fxpglobals']['dbparam'][$mcfi][$cname]))
			{
				$GLOBALS['fxpglobals']['dbparam'][$mcfi][$cname]=$val;
//echo('Adding field ['.$cname.'] to type ['.$mcfi.']: val='.$val.'<br />');
			}
		}
	}
}

/**
 * Get current table structure for one specified table and return information, or get information for all tables in global array variable $GLOBALS['_dbta']
 *
 * @param string  $tablename        - Optional parameter (default = '*'):    Get information for specific table 'tablename' or all '*'
 * @param boolean $with_columns     - Optional parameter (default = true):   Get also columns for each table?
 * @param boolean $with_primarykeys - Optional parameter (default = true):   Get also primary keys for each table?
 * @param boolean $with_indexes     - Optional parameter (default = true):   Get also indexes for each table?
 * @param boolean $display_progress - Optional parameter (default = false):  Display progress bar?
 *
 * @return Array of specified table on succes or false on error
 */
function dbStructure($tablename='*', $with_columns=true, $with_primarykeys=true, $with_indexes=true, $display_progress=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dtc=0;
	$dta=array(0=>"UNK", "SYS", "DAT", "INT", "IND");
	$tablename=strtolower($tablename);

	if(!isset($GLOBALS['_dbta']) || ($tablename == '*'))
		$GLOBALS['_dbta']=array();

	// Get table(s), except mask generator "fg%" and/or sequence "seq%" table(s)
	$sql="";
	if($GLOBALS['_dbtype'] == 'sqlsrv')
	{
		$sql  = "SELECT object_id AS id, name FROM sys.objects";
		$sql .= " WHERE type='U' AND name ";
		if($tablename != '*')
			$sql .= "LIKE '".$tablename."'";
		else
			$sql .= "NOT LIKE 'seq%' AND name NOT LIKE 'fg%' AND name NOT LIKE 'dtproperties' AND name NOT LIKE 'sysdiagrams'";
		if($tablename == '*')
			$sql .= " ORDER BY name";
	}
	else if($GLOBALS['_dbtype'] == 'pgsql')
	{
		$sql  = "SELECT oid AS id, relname AS name FROM pg_class";
		$sql .= " WHERE relkind='r' AND LOWER(relname) ";
		if($tablename != '*')
			$sql .= "LIKE '".$tablename."'";
		else
			$sql .= "NOT LIKE 'pg%' AND LOWER(relname) NOT LIKE 'sql%' AND LOWER(relname) NOT LIKE 'seq%' AND LOWER(relname) NOT LIKE 'fg%'";
		if($tablename == '*')
			$sql .= " ORDER BY LOWER(relname)";
	}
	if(strlen($sql))
	{
		$tat=db_values($sql,'*');
//fxDebug($tat,$sql);
		if(fxIsArray($tat))
		{
			$dtc=sizeof($tat);
			foreach($tat as $t)
			{
				$tn=strtolower($t['name']);
				$hf=0;
				if(substr($tn,-7) == 'history')
					$hf=1;
				$GLOBALS['_dbta'][$tn]=array('dbtid'=>$t['id'], 'id'=>0, 'type'=>$dta[0], 'client'=>false, 'history'=>$hf, 'columns'=>array(), 'primarykey'=>array(), 'indexes'=>array());
			}
		}
	}

	// Get table id's and types
	if(fxIsArray($GLOBALS['_dbta']))
	{
		$tex=true;
		if(!isset($GLOBALS['_dbta']['tabellen']))
			$tex=db_check('tabellen','t');
		if($tex)
		{
			$sql="SELECT id_tabellen, tabname, tabelleart, historie FROM tabellen";
			if($tablename != '*')
				$sql .= " WHERE ".db_like('tabname', $tablename);
			$tat=db_values($sql,'*');
//fxDebug($tat,$sql);
			if(fxIsArray($tat))
			{
				$dcc=1;
				foreach($tat as $t)
				{
					$tn=strtolower($t['tabname']);
					if($display_progress)
						fxProgressStatus('|Determining id and type for table &quot;'.$tn.'&quot; &hellip;|'.ceil($dcc*100/$dtc));
					if(isset($GLOBALS['_dbta'][$tn]))
					{
						$GLOBALS['_dbta'][$tn]['id']=$t['id_tabellen'];
						$GLOBALS['_dbta'][$tn]['type']=$dta[$t['tabelleart']];
						if($t['historie'] && !$GLOBALS['_dbta'][$tn]['history'])
							$GLOBALS['_dbta'][$tn]['history']=-1;
					}
					$dcc++;
				}
			}
		}
	}
//fxDebug($GLOBALS['_dbta'],'$GLOBALS[\'_dbta\']');

	// Get table columns
	if($with_columns && fxIsArray($GLOBALS['_dbta']))
	{
		dbGetColumns();
		$dcc=1;
		foreach($GLOBALS['_dbta'] as $tn => $td)
		{
			if($display_progress)
				fxProgressStatus('|Determining columns for table &quot;'.$tn.'&quot; &hellip;|'.ceil($dcc*100/$dtc));
			if(($tablename == '*') || ($tablename == $tn))
			{
				if($GLOBALS['_dbtype'] == 'sqlsrv')
				{
					$sql  = "SELECT c.name, t.name AS ctype, c.max_length, c.precision, c.scale, c.is_nullable";
					$sql .= " FROM sys.columns c INNER JOIN sys.types t ON t.user_type_id=c.system_type_id";
					$sql .= " WHERE c.object_id=".$td['dbtid'];
					$sql .= " ORDER BY c.column_id";
					$tat=db_values($sql,'*');
//fxDebug($tat,$sql);
					if(fxIsArray($tat))
					{
						foreach($tat as $t)
						{
							$cname=strtolower($t['name']);
							if($cname === 'mandanten_id')
								$GLOBALS['_dbta'][$tn]['client']=true;

							$ct=strtoupper($t['ctype']);
							if($ct == 'VARCHAR')
								$ctype='C'.$t['max_length'];
							else if($ct == 'DECIMAL')
								$ctype='D'.$t['precision'].','.$t['scale'];
							else
								$ctype=substr($ct,0,1);

							$cnn=1;
							if($t['is_nullable'])
								$cnn=0;

							$id=0;
							$itype=0;
							$ftype='';
							if(isset($GLOBALS['_dbca'][$cname]))
							{
								$id=$GLOBALS['_dbca'][$cname]['id'];
								$itype=$GLOBALS['_dbca'][$cname]['itype'];
								$ftype=$GLOBALS['_dbca'][$cname]['ftype'];
							}

							if(($itype == 2) || ($cname == 'autoid'))
								$ctype='S';
							else if(($ctype == 'I') && $itype)
							{
								if($itype == 4)
									$ctype .= '2';
								else
									$ctype .= '8';
							}

							$GLOBALS['_dbta'][$tn]['columns'][$cname]=array('id'=>$id, 'itype'=>$itype, 'ftype'=>$ftype, 'type'=>$ctype, 'not_null'=>$cnn);
						}
					}
				}
				else if($GLOBALS['_dbtype'] == 'pgsql')
				{
					$sql  = "SELECT attname, attnotnull, format_type(atttypid,atttypmod) AS ctype FROM pg_attribute";
					$sql .= " WHERE attrelid=".$td['dbtid']." AND attnum>0 AND NOT attisdropped";
					$sql .= " ORDER BY attnum";
					$tat=db_values($sql,'*');
//fxDebug($tat,$sql);
					if(fxIsArray($tat))
					{
						foreach($tat as $t)
						{
							$cname=strtolower($t['attname']);
							if($cname === 'mandanten_id')
								$GLOBALS['_dbta'][$tn]['client']=true;

							$ct=strtoupper($t['ctype']);
							if($ct == 'SMALLINT')
								$ctype='I2';
							else if($ct == 'BIGINT')
								$ctype='I8';
							else if(substr($ct,0,18) === 'CHARACTER VARYING(')
								$ctype='C'.(int)substr($ct,18);
							else if(substr($ct,0,8) === 'NUMERIC(')
								$ctype='D'.substr($ct,8,-1);
							else
								$ctype=substr($ct,0,1);

							$cnn=0;
							if($t['attnotnull'] == 't')
								$cnn=1;

							$id=0;
							$itype=0;
							$ftype='';
							if(isset($GLOBALS['_dbca'][$cname]))
							{
								$id=$GLOBALS['_dbca'][$cname]['id'];
								$itype=$GLOBALS['_dbca'][$cname]['itype'];
								$ftype=$GLOBALS['_dbca'][$cname]['ftype'];
							}

							if(($itype == 2) || ($cname == 'autoid'))
								$ctype='S';

							$GLOBALS['_dbta'][$tn]['columns'][$cname]=array('id'=>$id, 'itype'=>$itype, 'ftype'=>$ftype, 'type'=>$ctype, 'not_null'=>$cnn);
						}
					}
				}
				dbAddTableTypes($tn);
			}
			$dcc++;
		}
	}

	// Get primary keys
	if($with_primarykeys && fxIsArray($GLOBALS['_dbta']))
	{
		$dcc=1;
		foreach($GLOBALS['_dbta'] as $tn => $td)
		{
			if($display_progress)
				fxProgressStatus('|Determining primary keys table &quot;'.$tn.'&quot; &hellip;|'.ceil($dcc*100/$dtc));
			if(($tablename == '*') || ($tablename == $tn))
			{
				if($GLOBALS['_dbtype'] == 'sqlsrv')
				{
					$sql  = "SELECT i.name AS pkname, i.type_desc, COL_NAME(c.object_id,c.column_id) AS cname";
					$sql .= " FROM sys.indexes i INNER JOIN sys.index_columns c ON c.object_id=i.object_id AND c.index_id=i.index_id";
					$sql .= " WHERE i.object_id=".$td['dbtid']." AND i.is_primary_key=1";
					$tat=db_values($sql,'*');
//fxDebug($tat,$sql);
					if(fxIsArray($tat))
					{
						foreach($tat as $t)
						{
							if(!sizeof($GLOBALS['_dbta'][$tn]['primarykey']))
							{
								$cl=false;
								if(strtoupper(substr($t['type_desc'],0,1)) == 'C')
									$cl=true;

								$GLOBALS['_dbta'][$tn]['primarykey']=array('name'=>$t['pkname'], 'clustered'=>$cl, 'columns'=>strtolower($t['cname']));
							}
							else
								$GLOBALS['_dbta'][$tn]['primarykey']['columns'] .= ','.strtolower($t['cname']);
						}
					}
				}
				else if($GLOBALS['_dbtype'] == 'pgsql')
				{
					$sql  = "SELECT c.relname AS pkname, i.indisclustered, a.attname AS cname";
					$sql .= " FROM pg_class c, pg_index i JOIN pg_attribute a ON a.attrelid=i.indrelid AND a.attnum=ANY(i.indkey)";
					$sql .= " WHERE i.indrelid=".$td['dbtid']." AND i.indisprimary";
					$sql .= " AND i.indexrelid=c.oid";
					$tat=db_values($sql,'*');
//fxDebug($tat,$sql);
					if(fxIsArray($tat))
					{
						foreach($tat as $t)
						{
							$cl=false;
							if($t['indisclustered'] == 't')
								$cl=true;

							if(!sizeof($GLOBALS['_dbta'][$tn]['primarykey']))
								$GLOBALS['_dbta'][$tn]['primarykey']=array('name'=>$t['pkname'], 'clustered'=>$cl, 'columns'=>strtolower($t['cname']));
							else
								$GLOBALS['_dbta'][$tn]['primarykey']['columns'] .= ','.strtolower($t['cname']);
						}
					}
				}
			}
			$dcc++;
		}
	}

	// Get indexes
	if($with_indexes && fxIsArray($GLOBALS['_dbta']))
	{
		$dcc=1;
		foreach($GLOBALS['_dbta'] as $tn => $td)
		{
			if($display_progress)
				fxProgressStatus('|Determining indexes for table &quot;'.$tn.'&quot; &hellip;|'.ceil($dcc*100/$dtc));
			if(($tablename == '*') || ($tablename == $tn))
			{
				if($GLOBALS['_dbtype'] == 'sqlsrv')
				{
					$sql  = "SELECT i.name AS idxname, i.type_desc, COL_NAME(c.object_id,c.column_id) AS cname";
					$sql .= " FROM sys.indexes i INNER JOIN sys.index_columns c ON c.object_id=i.object_id AND c.index_id=i.index_id";
					$sql .= " WHERE i.object_id=".$td['dbtid']." AND i.is_primary_key=0";
					$tat=db_values($sql,'*');
//fxDebug($tat,$sql);
					if(fxIsArray($tat))
					{
						foreach($tat as $t)
						{
							$iname=$t['idxname'];
							if(!isset($GLOBALS['_dbta'][$tn]['indexes'][$iname]))
							{
								$cl=false;
								if(strtoupper(substr($t['type_desc'],0,1)) == 'C')
									$cl=true;

								$GLOBALS['_dbta'][$tn]['indexes'][$iname]=array('clustered'=>$cl, 'columns'=>strtolower($t['cname']), 'dropped'=>false);
							}
							else
								$GLOBALS['_dbta'][$tn]['indexes'][$iname]['columns'] .= ','.strtolower($t['cname']);
						}
					}
				}
				else if($GLOBALS['_dbtype'] == 'pgsql')
				{
					$sql  = "SELECT c.relname AS idxname, i.indisclustered, a.attname AS cname";
					$sql .= " FROM pg_class c, pg_index i JOIN pg_attribute a ON a.attrelid=i.indrelid AND a.attnum=ANY(i.indkey)";
					$sql .= " WHERE i.indrelid=".$td['dbtid']." AND NOT i.indisprimary";
					$sql .= " AND i.indexrelid=c.oid";
					$tat=db_values($sql,'*');
//fxDebug($tat,$sql);
					if(fxIsArray($tat))
					{
						foreach($tat as $t)
						{
							$iname=$t['idxname'];
							if(!isset($GLOBALS['_dbta'][$tn]['indexes'][$iname]))
							{
								$cl=false;
								if($t['indisclustered'] == 't')
									$cl=true;

								$GLOBALS['_dbta'][$tn]['indexes'][$iname]=array('clustered'=>$cl, 'columns'=>strtolower($t['cname']), 'dropped'=>false);
							}
							else
								$GLOBALS['_dbta'][$tn]['indexes'][$iname]['columns'] .= ','.strtolower($t['cname']);
						}
					}
				}
			}
		}
		$dcc++;
	}

	if($tablename != '*')
	{
		if(fxIsArray($GLOBALS['_dbta'][$tablename]))
			return $GLOBALS['_dbta'][$tablename];
		return false;
	}
}

/**
 * ???FUNCTION???
 *
 * @param boolean $with_sql          - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $only_system_views - Optional parameter (default = false): ???PARAMETER???
 */
function dbViews($with_sql=true, $only_system_views=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['_dbva']=array();

	// Get all views
	$sql="";
	if($GLOBALS['_dbtype'] == 'sqlsrv')
		$sql="SELECT v.name, v.object_id AS id, m.definition AS sql FROM sys.views v INNER JOIN sys.sql_modules m ON m.object_id=v.object_id WHERE v.name LIKE 'v%' ORDER BY v.create_date";
	else if($GLOBALS['_dbtype'] == 'pgsql')
		$sql="SELECT c.relname AS name, c.oid AS id, v.definition AS sql FROM pg_class c INNER JOIN pg_views v ON v.viewname=c.relname WHERE c.relkind='v' AND c.relacl IS NULL AND c.relname LIKE 'v%' ORDER BY c.oid";

	if(strlen($sql))
	{
		$vat=db_values($sql,'*');
//fxDebug($vat,$sql);
		if(fxIsArray($vat))
		{
			$vca=1;
			$vcb=1;
			foreach($vat as $vd)
			{
				$vn=strtoupper($vd['name']);
				$v3=substr($vn,0,3);
				$v6=substr($vn,0,6);
				if(($v3 == 'VD_') || ($v3 == 'VN_') || ($v3 == 'VS_') || ($v6 == 'V_FXP_'))
				{
					$sn='A';
					$rf=false;
					$ri=0;
					if($v6 == 'V_FXP_')
					{
						$sn='B';
						$rf=true;
						$ri=(int)substr($vn,6);
					}
					if(!$rf || ($rf && (!$only_system_views || ($ri < 100))))
					{
						if($with_sql)
							$sql=dbFormatView($vd['sql'],true);
						else
							$sql="";

						if($rf)
						{
							$vc=$vcb;
							$vcb++;
						}
						else
						{
							$vc=$vca;
							$vca++;
						}
						$GLOBALS['_dbva'][$sn.'-'.substr('000000'.$vc,-6)]=array('dbvid'=>$vd['id'], 'name'=>$vn, 'repview'=>$rf, 'id'=>0, 'sql'=>$sql, 'dbsql'=>"", 'tables'=>array());
					}
				}
			}
		}
	}

	if(sizeof($GLOBALS['_dbva']))
	{
//fxDebug($GLOBALS['_dbva'],'$GLOBALS[\'_dbva\']');
		if($with_sql)
		{
			$rtable_exists=db_check('repviews','t');

			$sa=$GLOBALS['_dbva'];
			$GLOBALS['_dbva']=array();

			foreach($sa as $vs => $vd)
			{
				$sn=$vs;
				$vn=$vd['name'];
				$id=0;
				$sql="";

				if($vd['repview'] && $rtable_exists)
				{
					$rid=(int)substr($vn,6);
					$sn='1-'.substr('000000'.$rid,-6);
					$vsql="SELECT view_sql FROM repviews WHERE repviewid=".$rid." AND mandanten_id=0";
					$sql=db_value($vsql);
					if(strlen($sql))
					{
						$sql=dbFormatView($sql,true);
						$id=$rid;
					}
				}

				if($id)
					$vd['id']=$id;
				if(strlen($sql))
					$vd['dbsql']=$sql;

				$vd['tables']=dbViewTables($vd['sql']);

				$GLOBALS['_dbva'][$sn]=$vd;
			}
		}

		ksort($GLOBALS['_dbva'],SORT_STRING);
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $tp - Mandatory parameter:                     ???PARAMETER???
 * @param various $tn - Mandatory parameter:                     ???PARAMETER???
 * @param various $td - Mandatory parameter:                     ???PARAMETER???
 * @param integer $ai - Optional parameter (default = 0):        ???PARAMETER???
 * @param array   $ad - Optional parameter (default = array()):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbGetDefLine($tp, $tn, $td, $ai=0, $ad=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$tla=array('line'=>"", 'checksum'=>"");
	$vd=$GLOBALS['fxpglobals']['dbparam']['versionscode_db'];

	switch($tp)
	{
		case 'T':	// Table
			$cf='0';
			if($td['client'])
				$cf='1';
			$hf=$td['history'];

			// Columns
			$cc='';
			if(sizeof($td['columns']))
			{
				foreach($td['columns'] as $cn => $ca)
				{
					$nf='0';
					if($ca['not_null'])
						$nf='1';
					if(strlen($cc))
						$cc .= ';';
					$cc .= $cn.':'.$ca['id'].':'.$ca['itype'].':'.$ca['type'].':'.$nf;
				}
			}

			// Primary key
			$pk='';
			if(sizeof($td['primarykey']))
			{
				$lf='0';
				if($td['primarykey']['clustered'])
					$lf='1';
//				$pkn=$td['primarykey']['name'];
				if($td['id'] < -9999)
					$id='C'.abs($td['id']);
				else if($td['id'] < 0)
					$id='H'.abs($td['id']);
				else
					$id=$td['id'];
				$pkn='PK_'.(int)substr($vd,0,3).(int)substr($vd,3,3).(int)substr($vd,6,3).'_'.$id;
				$pk=$pkn.':'.$td['primarykey']['columns'].':'.$lf;
			}

			$tla['line']=$tn.'|'.$td['id'].'|'.$td['type'].'|'.$cf.'|'.$hf.'|'.$cc.'|'.$pk;
			$tla['checksum']=lineCode($tla['line'], $tn);
		break;

		case 'I':	// Index
			$lf='0';
			if($ad['clustered'])
				$lf='1';
			if($td['id'] < -9999)
				$id='C'.abs($td['id']);
			else if($td['id'] < 0)
				$id='H'.abs($td['id']);
			else
				$id=$td['id'];
			$in='IDX_'.(int)substr($vd,0,3).(int)substr($vd,3,3).(int)substr($vd,6,3).'_'.$id;
			if($ai > 0)
				$in .= '_'.$ai;

			$tla['line']=$tn.'|'.$td['id'].'|'.$in.'|'.$ad['columns'].'|'.$lf;
			$tla['checksum']=lineCode($tla['line'], $in);
		break;

		case 'D':	// Data
			if($ai < 0)		// ...Header for Patch
			{
				$tcs=sizeof($ad);

				$tla['line']=$tn.'|'.$td['id'].'|'.$tcs.'|'.$td['datcolumns'];
				$tla['checksum']=lineCode($tla['line'], $tn.'-'.$tcs);
			}
			else if(!$ai)	// ...Header for Defintion
			{
				$tcs=sizeof($ad);

				$cc='';
				foreach($ad[0] as $cn => $cv)
				{
					if(strlen($cc))
						$cc .= ',';
					$cc .= $cn;
				}

				$tla['line']=$tn.'|'.$td['id'].'|'.$tcs.'|'.$cc;
				$tla['checksum']=lineCode($tla['line'], $tn.'-'.$tcs);
			}
			else		// ...Data
			{
				$tra=dbGetDefRepl();

				$cc='';
				if(!fxIsArray($ad))
					$cc=$ad;
				else
				{
					foreach($ad as $cn => $cv)
					{
						if(strlen($cc))
							$cc .= ',';
						if(strlen($cv))
						{
							if(is_int($cv) || is_float($cv))
								$cc .= $cv;
							else
							{
								if(substr($cv,0,1) == '§')
									$cv=substr($cv,1);
								if(strlen($cv))
									$cc .= "'".strtr(convert_string($cv,'todb'),$tra)."'";
							}
						}
					}
				}
				$tla['line']=$cc;
				$tla['checksum']=lineCode($tla['line'], $tn.'-'.$ai);
			}
		break;
	}

	return $tla;
}

/**
 * ???FUNCTION???
 *
 * @param various $vsql      - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $tolower   - Optional parameter (default = false):  ???PARAMETER???
 * @param boolean $replacews - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbFormatView($vsql, $tolower=false, $replacews=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$vsql=trim(str_replace('  ', ' ', strtr($vsql,array("\r\n"=>' ', "\r"=>' ', "\n"=>' '))));
	if(substr($vsql,0,2) == '/'.'*')
	{
		$vsql=substr($vsql,2);
		$slp=strpos($vsql,'/');
		$vsql=trim(substr($vsql,$slp+1));
	}
	if(strtolower(substr($vsql,0,12)) == 'create view ')
	{
		$ap=stripos($vsql,' as ', 12);
		if($ap)
			$vsql=trim(substr($vsql,$ap+4));
	}
	if($tolower)
		$vsql=strtolower($vsql);
	if($replacews)
	{
		$vsql=trim(strtr($vsql,array("inner"=>'', "cast"=>'', "as char"=>'', " "=>'', ";"=>'', "("=>'', ")"=>'')));
		$vsql=trim(strtr($vsql,array("::text"=>'', "::character1"=>'', "like"=>'~~')));
	}

	return $vsql;
}

/**
 * ???FUNCTION???
 *
 * @param various $sql - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbReplaceCommaNull($sql)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	while(true)
	{
		$dcf=strpos($sql,",,");
		if($dcf === false)
			break;

		$sql=str_replace(",,", ",NULL,", $sql);
	}
	$sql=str_replace(",", ", ", $sql);

	return $sql;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $to_file    - Optional parameter (default = true): ???PARAMETER???
 * @param boolean $with_comma - Optional parameter (default = true): ???PARAMETER???
 * @param boolean $with_pipe  - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbGetDefRepl($to_file=true, $with_comma=true, $with_pipe=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$nrs='(*N*)';
	$crs='(-C-)';
	$prs='(+P+)';

	if($to_file)
	{
		$tra=array("\r\n"=>$nrs, "\r"=>$nrs, "\n"=>$nrs);
		if($with_comma)
			$tra[","]=$crs;
		if($with_pipe)
			$tra["|"]=$prs;
	}
	else
	{
		$tra=array($nrs=>$GLOBALS['nl']);
		if($with_comma)
			$tra[$crs]=",";
		if($with_pipe)
			$tra[$prs]="|";
	}

	return $tra;
}

/**
 * ???FUNCTION???
 *
 * @param various $tablename    - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $client_table - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $columns      - Optional parameter (default = '*'):    ???PARAMETER???
 * @param string  $pk_columns   - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbGetDataSQL($tablename, $client_table=false, $columns='*', $pk_columns='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$special_tables=array(
		'felder'			=> "id>0",
		'masken'			=> "nummer>0",
		'maskenfelder'		=> "maskenfeldid>0",
		'tabellen'			=> "id_tabellen>-10000",
		'tab_felder'		=> "idintabellen>-10000",
		'texte'				=> "id_referenz>0",
		'trans'				=> "transakt_id>0",
		'trans_aktionen'	=> "trans_id>0",
		'trans_maske'		=> "id_transmaske>0",
		'wertetab'			=> "wertetabid>0"
	);

	$sql="SELECT ".$columns." FROM ".$tablename;
	$where='';
	if($client_table)
	{
		if(strlen($where))
			$where .= " AND";
		$where .= " mandanten_id=0";
	}
	if(isset($special_tables[$tablename]))
	{
		if(strlen($where))
			$where .= " AND";
		$where .= " ".$special_tables[$tablename];
	}
	if(strlen($where))
		$sql .= " WHERE".$where;
	// ...Primarykey
	if(strlen($pk_columns))
		$sql .= " ORDER BY ".$pk_columns;
//echo('<b>'.$tablename.'</b>: '.$sql.'<br />');

	return $sql;
}

/**
 * mode: -1=Create patch, 0=Execute update, 1=Execute patch
 *
 * @param various $vd   - Mandatory parameter:                     ???PARAMETER???
 * @param array   $fa   - Optional parameter (default = array()):  ???PARAMETER???
 * @param integer $mode - Optional parameter (default = 1):        ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbGetTablePKDiff($vd, $fa=array(), $mode=1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pa=array(
		'status'		=> 0,
		'message'		=> '',
		'screencontent'	=> '',
		'filename'		=> '',
		'filelines'		=> 0,
		'rellines'		=> 0,
		'changelines'	=> 0,
		'filecontent'	=> '',
		'tablearray'	=> array(),
		'sqlarray'		=> array()
	);

	$create=false;
	if($mode < 0)
		$create=true;
	else if(!isset($GLOBALS['_dbta']) || (!isset($GLOBALS['fxinstall']) && !isset($GLOBALS['fxupdate'])))
		$GLOBALS['_dbta']=array();

	if($mode == 1)
	{
		$pa['filename']=$GLOBALS['datpath'].$vd.'_pat.txt';
		$ptype=' <i>(Patch)</i>';
		$vpcod='PATCH';
	}
	else
	{
		$pa['filename']=$GLOBALS['datpath'].$vd.'_vrs.txt';
		$ptype='';
		$vpcod='VERSION';
	}

	$vp=(int)substr($vd,0,3).'.'.(int)substr($vd,3,3).'.'.(int)substr($vd,6,3);

	$fas=false;
	if(fxIsArray($fa))
		$fas=true;

	// Reading table structure file
	if(!$fas)
		$pa['screencontent'] .= 'Reading file &quot;'.$pa['filename'].'&quot; for <b>Tables</b>'.$ptype.' in version <b>'.$vp.'</b>';
	if($fas || file_exists($pa['filename']))
	{
		if(!$fas)
			$fa=fxLoad($pa['filename'], 1);
		if(fxIsArray($fa))
		{
			$pa['filelines']=sizeof($fa);
			if(!$fas)
				$pa['screencontent'] .= dbStatus(1, 'Read '.$pa['filelines'].' lines.').'<br />'.$GLOBALS['nl'];

			$gc=0;
			$ko='';

			if(!$fas)
			{
				$pa['screencontent'] .= '<!--#CHECK#-->';

				// ...Headline
				$ha=dbGetHeader($fa);
//fxDebug($ha,'$ha', 0);
				$pa['screencontent'] .= '&nbsp;&nbsp;&nbsp;<b>&middot;</b>&nbsp;Checking header';
				if(!strlen($ha['filename']) || !strlen($ha['version']) || !strlen($ha['versioncode']) || !strlen($ha['date']) || !strlen($ha['time']) || !strlen($ha['checksum']))
					$ko='TH.1 - Header parameter missing';
				if(!strlen($ko) && (strlen($ha['filename']) != 9))
					$ko='TH.2 - Wrong filename parameter length';
				if(!strlen($ko) && ((($mode != 1) && (strlen($ha['versioncode']) != 9)) || (($mode == 1) && (strlen($ha['versioncode']) != 13))))
					$ko='TH.3 - Wrong versioncode parameter length';
				if(!strlen($ko))
				{
					$hd=$vpcod.'-'.$ha['versioncode'].'-'.str_replace('-', '', $ha['date']).str_replace(':', '', $ha['time']);
					$h5=lineCode($hd, $vpcod);
					if($ha['checksum'] !== $h5)
						$ko='TH.4 - Checksum mismatch';
				}
				if(strlen($ko))
				{
					$pa['status']=-1;
					$pa['message']=$ko.'!';
					$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
				}
				else
					$pa['screencontent'] .= dbStatus(1).'<br />'.$GLOBALS['nl'];
			}

			// ...Tables
			// *T|abhaengigkeiten|204|DAT|1|1|mandanten_id:22:5:I8:1,projekt_a:2001:5:I8:1,projekt_b:2002:5:I8:1,abh_typ:2003:1:C2:1,tagesanzahl:921:5:I8:0,historie:1729:4:I2:0,transid:12:5:I8:0,aenderungs_id:84:5:I8:1,zeitstempel:19:10:C14:1|PK_530_204:mandanten_id,projekt_a,projekt_b,abh_typ:0|24297824e30041f6b3d48537aaa7a851
			$fpka=array();
			$fvwa=array();
			$fcta=array();
			if(!strlen($ko))
			{
				// Views
				if(!fxIsArray($GLOBALS['_dbva']))
					dbViews();
//fxDebug($GLOBALS['_dbva'],'$GLOBALS[\'_dbva\']');

				if(!$fas)
					$pa['screencontent'] .= '&nbsp;&nbsp;&nbsp;<b>&middot;</b>&nbsp;Checking tables:';
				$dbta=array();
				$cst='';
				foreach($fa as $fc => $fl)
				{
					if($fc)
					{
						// Validity check
						$fl=trim($fl);
						$la=explode('|', $fl);
//fxDebug($la,'$la: '.fxHtmlEncode($fl));

						// Do we have a table [*T] line?
						if($la[0] === '*T')
						{
							$pa['rellines']++;
							$pkn='';
							$pknm='';
							$pkcl=false;
							$ttca=array();
							$ttpa=array();

							// ...Line must contain exactly 9 data entries
							if(sizeof($la) != 9)
								$ko='TL'.$fc.'.1';
							else
							{
								foreach($la as $ln => $lv)
									$la[$ln]=trim($lv);
							}

							// ...1st entry must be table definition identifier [*T]
							if(!strlen($ko) && ($la[0] !== '*T'))
								$ko='TL'.$fc.'.2';

							// ...Checksum must match
							if(!strlen($ko))
							{
								$ltn=$la[1];
								$tl=substr($fl,3,-33);
								$t5=lineCode($tl, $ltn);
								if($la[8] !== $t5)
									$ko='TL'.$fc.'.3';
							}

							// Check changes
							if(!strlen($ko))
							{
								$dbta[$ltn]=true;

								// ...Format integer types: 2=table id, 4=client flag + 5=history flag
								$la[2]=(int)$la[2];
								$la[4]=(int)$la[4];
								$la[5]=(int)$la[5];

								// Columns
								if(strlen($la[6]))
								{
									$ca=explode(';', $la[6]);
									foreach($ca as $ct)
									{
										$cda=explode(':', $ct);
										// ...Format integer types: 1=field id, 2=field type + 4=not null flag
										$cda[1]=(int)$cda[1];
										$cda[2]=(int)$cda[2];
										$cda[4]=(int)$cda[4];
										$ttca[strtolower(trim($cda[0]))]=$cda;
									}
								}

								// Primarykeys
								if(strlen($la[7]))
								{
									$pkta=explode(':', $la[7]);
									if(sizeof($pkta) == 3)
									{
										$pknm=trim($pkta[0]);
										if((int)$pkta[2])
											$pkcl=true;
										$pkta=explode(',', $pkta[1]);
										foreach($pkta as $pktc)
											$ttpa[strtolower(trim($pktc))]=true;
									}

									if(sizeof($ttpa))
									{
										$pktid=$la[2];
										if($pktid < 0)
											$pktid='H'.abs($pktid);
										$pkn=dbCreatePrimaryKeyDef('PK_'.(int)substr($vd,0,3).(int)substr($vd,3,3).(int)substr($vd,6,3).'_'.$pktid, $pkcl, $ttpa);
									}
								}

								$c=0;
								$ci='';
								// Exists table?
								$tex=true;
								$rtn=$ltn;
								if(!isset($GLOBALS['_dbta'][$ltn]) && !isset($GLOBALS['fxinstall']) && !isset($GLOBALS['fxupdate']))
									dbStructure($ltn);
								if(!isset($GLOBALS['_dbta'][$ltn]))
								{
									$ren=false;
									foreach($GLOBALS['_dbta'] as $ttn => $tda)
									{
										if($tda['id'] == $la[2])
										{
											$c++;	// Table has been renamed
											if($create)
												$ci .= ', Table has been renamed from ['.$ltn.'] to ['.$ttn.']';
											else
												$ci .= ', Table has been renamed from ['.$ttn.'] to ['.$ltn.']';
											$rtn=$ttn;
											$ren=true;

											if(!$create)
											{
												// Drop Views?
												$dvsa=dbCheckDropViews($fvwa, $ttn,$ltn);
												if(fxIsArray($dvsa))
												{
													foreach($dvsa as $vid => $sql)
													{
														$fvwa[$vid]=array(0=>$ttn, 1=>$ltn);
														$pa['sqlarray'][]=$sql;
													}
												}
												if($GLOBALS['_dbtype'] == 'sqlsrv')
													$sql="EXEC sp_rename 'dbo.".ttn."', '".$ltn."'";
												else
													$sql="ALTER TABLE ".$ttn." RENAME TO ".$ltn;

												$pa['sqlarray'][]=$sql;
											}

											break;
										}
									}
									if(!$ren)
									{
										$c++;	// Table not found
										$ci .= ', Table ['.$ltn.'] not found';
										$tex=false;

										if(!$create)
										{
											$sql=dbCreateTableSQL($ltn, $ttca, $pkn, $ttpa);
											$pa['sqlarray'][]=$sql;
//echo('<pre>'.fxHtmlEncode($sql).'</pre>'.$GLOBALS['nl']);
										}
									}
								}
								if($tex)
								{
									$tla=dbGetDefLine('T', $rtn, $GLOBALS['_dbta'][$rtn]);
									if($tl !== $tla['line'])
									{
/*
										if($GLOBALS['_dbta'][$rtn]['type'] != $la[3])
										{
											$c++;	// Table type has changed
											$ci .= ', Table type has changed from ['.$GLOBALS['_dbta'][$rtn]['type'].'] to ['.$la[3].']';
										}
										$hf=$GLOBALS['_dbta'][$rtn]['history'];
										if($hf != $la[5])
										{
											$c++;	// Table history flag has changed
											$ci .= ', Table history flag has changed from ['.$hf.'] to ['.$la[5].']';
										}
*/

										// Check columns
										if(sizeof($ttca))
										{
											foreach($ttca as $cn => $cda)
											{
												$cex=true;
												$rcn=$cn;

												$bctdb=substr($GLOBALS['_dbta'][$rtn]['columns'][$rcn]['type'],0,1);
												$bctfl=substr($cda[3],0,1);

												if(!isset($GLOBALS['_dbta'][$rtn]['columns'][$cn]))
												{
													$cren=false;
													if($cda[1] && fxIsArray($GLOBALS['_dbca'][$rtn]['columns']))
													{
														foreach($GLOBALS['_dbca'][$rtn]['columns'] as $tcn => $tca)
														{
															if($tca['id'] == $cda[1])
															{
																$c++;	// Column has been renamed
																$ci .= ', Column has been renamed from ['.$tcn.'] to ['.$cn.']';
																$cren=true;
																$rcn=$tcn;

																if(!$create)
																{
																	// Drop Views?
																	$dvsa=dbCheckDropViews($fvwa, $ltn);
																	if(fxIsArray($dvsa))
																	{
																		foreach($dvsa as $vid => $sql)
																		{
																			$fvwa[$vid]=array(0=>$ltn);
																			$pa['sqlarray'][]=$sql;
																		}
																	}
																	if($GLOBALS['_dbtype'] == 'sqlsrv')
																		$sql="EXEC sp_rename 'dbo.".$ltn.".".$tcn."', '".$cn."', 'COLUMN'";
																	else
																		$sql="ALTER TABLE ".$ltn." RENAME COLUMN ".$tcn." TO ".$cn;
																	$pa['sqlarray'][]=$sql;
																}

																break;
															}
														}
													}
													if(!$cren)
													{
														$c++;	// Column not found
														$ci .= ', Column ['.$cn.'] not found';
														$cex=false;

														if(!$create)
														{
															$dvsa=dbCheckDropViews($fvwa, $ltn);
															if(fxIsArray($dvsa))
															{
																foreach($dvsa as $vid => $sql)
																{
																	$fvwa[$vid]=array(0=>$ltn);
																	$pa['sqlarray'][]=$sql;
																}
															}
															$sql="ALTER TABLE ";
															if($GLOBALS['_dbtype'] == 'sqlsrv')
																$sql .= "dbo.";
															$sql .= $ltn." ADD ".$cn." ".dbColumnType($cda[3]);
															// NOT NULL?
															if($cda[4] || isset($ttpa[$cn]))
															{
																$sql .= " NOT NULL";
																if($bctfl != 'S')
																{
																	$sql .= " DEFAULT (";
																	if(($bctfl == 'I') || ($bctfl == 'D'))
																		$sql .= "0";
																	else
																		$sql .= "''";
																	$sql .= ")";
																}
															}
															else if($GLOBALS['_dbtype'] == 'sqlsrv')
																$sql .= " NULL";

															$pa['sqlarray'][]=$sql;
//echo('<pre>'.fxHtmlEncode($sql).'</pre>'.$GLOBALS['nl']);
														}
													}
												}

												$tpc=false;
												if($GLOBALS['_dbta'][$rtn]['columns'][$rcn]['type'] != $cda[3])
													$tpc=true;
												$nnc=false;
												if($GLOBALS['_dbta'][$rtn]['columns'][$rcn]['not_null'] != $cda[4])
													$nnc=true;
												if($cex && ($tpc || $nnc))
												{
													$c++;	// Column type and/or nor null has changed
													if($create)
														$ci .= ', Type and/or NotNull for column ['.$cn.'] has changed from ['.$cda[3].']-NN['.$cda[4].'] to ['.$GLOBALS['_dbta'][$rtn]['columns'][$rcn]['type'].']-NN['.$GLOBALS['_dbta'][$rtn]['columns'][$rcn]['not_null'].']';
													else
														$ci .= ', Type and/or NotNull for column ['.$cn.'] has changed from ['.$GLOBALS['_dbta'][$rtn]['columns'][$rcn]['type'].']-NN['.$GLOBALS['_dbta'][$rtn]['columns'][$rcn]['not_null'].'] to ['.$cda[3].']-NN['.$cda[4].']';

													if(!$create)
													{
//if($rtn == 'kommunikation') fxDebug($GLOBALS['_dbta'][$rtn], $rtn);
														$dvsa=dbCheckDropViews($fvwa, $ltn);
														if(fxIsArray($dvsa))
														{
															foreach($dvsa as $vid => $sql)
															{
																$fvwa[$vid]=array(0=>$ltn);
																$pa['sqlarray'][]=$sql;
															}
														}

														if(fxIsArray($GLOBALS['_dbta'][$rtn]['indexes']))
														{
															foreach($GLOBALS['_dbta'][$rtn]['indexes'] as $tin => $tia)
															{
																$cii=stripos($tia['columns'],$cn);
																if(!$tia['dropped'] && ($cii !== false))
																{
																	$sql="DROP INDEX ".$tin;
																	if($GLOBALS['_dbtype'] == 'sqlsrv')
																		$sql .= " ON dbo.".$rtn;
																	$sql .= $GLOBALS['fxpglobals']['dbparam']['dbcascade'];
//echo('<pre>'.$sql.'</pre>'.$GLOBALS['nl']);
																	$pa['sqlarray'][]=$sql;
																	$GLOBALS['_dbta'][$rtn]['indexes'][$tin]['dropped']=true;
																}
															}
														}

														// Column type has changed?
														if($tpc)
														{
															$sql="ALTER TABLE ";
															if($GLOBALS['_dbtype'] == 'sqlsrv')
																$sql .= "dbo.";
															$sql .= $ltn." ALTER COLUMN ".$cn;
															if($GLOBALS['_dbtype'] == 'pgsql')
																$sql .= " TYPE";
															$sql .= " ".dbColumnType($cda[3]);
//echo('<pre>'.$sql.'</pre>'.$GLOBALS['nl']);

															$pa['sqlarray'][]=$sql;
														}

														// Column not null has changed?
														if($nnc)
														{
															if($cda[4] && ($bctfl != 'S'))
															{
																$sql="UPDATE ".$ltn." SET ".$cn."=";
																if(($bctfl == 'I') || ($bctfl == 'D'))
																	$sql .= "0";
																else
																	$sql .= "''";
																$sql .= " WHERE ".$cn." IS NULL";
																if($GLOBALS['_dbta'][$rtn]['client'])
																	$sql .= " AND mandanten_id IS NOT NULL";
//echo('<pre>'.$sql.'</pre>'.$GLOBALS['nl']);

																$pa['sqlarray'][]=$sql;
															}

															$sql="ALTER TABLE ";
															if($GLOBALS['_dbtype'] == 'sqlsrv')
																$sql .= "dbo.";
															$sql .= $ltn." ALTER COLUMN ".$cn." ";
															if($GLOBALS['_dbtype'] == 'pgsql')
															{
																if($cda[4])
																	$sql .= "SET";
																else
																	$sql .= "DROP";
																$sql .= " NOT NULL";
															}
															else
															{
																$sql .= dbColumnType($cda[3])." ";
																if($cda[4])
																	$sql .= "NOT ";
																$sql .= "NULL";
															}
//echo('<pre>'.$sql.'</pre>'.$GLOBALS['nl']);

															$pa['sqlarray'][]=$sql;
														}
													}
												}
											}

											if($create)
											{
												foreach($GLOBALS['_dbta'][$rtn]['columns'] as $cn => $ctd)
												{
													if(!isset($ttca[$cn]))
													{
														$c++;	// Column has been deleted
														$ci .= ', Column ['.$cn.'] has been added';
													}
												}
												if(sizeof($ttca))
												{
													foreach($ttca as $cn => $ctd)
													{
														if(!isset($GLOBALS['_dbta'][$rtn]['columns'][$cn]))
														{
															$c++;	// Column has been deleted
															$ci .= ', Column ['.$cn.'] has been deleted';
														}
													}
												}
											}
											else
											{
												foreach($GLOBALS['_dbta'][$rtn]['columns'] as $cn => $ctd)
												{
													if(!isset($ttca[$cn]))
													{
														$c++;	// Column has been deleted
														$ci .= ', Column ['.$cn.'] has been deleted';

														if(!$create)
														{
															// TODO: Check for column dependencies and delete them

															$dvsa=dbCheckDropViews($fvwa, $ltn);
															if(fxIsArray($dvsa))
															{
																foreach($dvsa as $vid => $sql)
																{
																	$fvwa[$vid]=array(0=>$ltn);
																	$pa['sqlarray'][]=$sql;
																}
															}
															$sql="ALTER TABLE ";
															if($GLOBALS['_dbtype'] == 'sqlsrv')
																$sql .= "dbo.";
															$sql .= $ltn." DROP COLUMN ".$cn;
															$sql .= $GLOBALS['fxpglobals']['dbparam']['dbcascade'];

															$pa['sqlarray'][]=$sql;
														}
													}
												}
											}
										}

										// Save primary key for below check
										if(sizeof($GLOBALS['_dbta'][$rtn]['primarykey']) || strlen($la[7]))
											$fpka[$rtn]=array('fc'=>$fc, 'tpk'=>$GLOBALS['_dbta'][$rtn]['primarykey'], 'fpk'=>$la[7], 'fpa'=>$ttpa, 'pkn'=>$pkn);
									}
								}

								if($c)
								{
									$gc++;
									$cpl='';
									if($c > 1)
										$cpl='s';
									$cst .= '<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;<b>'.$ltn.'</b>'.dbStatus(1, '^<b>'.$ltn.'</b><hr size=1 /><br />'.substr($ci,2), $c.' change'.$cpl);

									$fcta[$ltn]=true;

									$fl='*T|'.$tla['line'].'|'.$tla['checksum'];
									$pa['filecontent'] .= $fl.$GLOBALS['nl_b'];
									$pa['tablearray'][$ltn]='changed';
									$pa['changelines']++;
								}
							}
						}
					}

					if(strlen($ko))
						break;
				}

				// Add new tables
				if(!strlen($ko) && $create && sizeof($GLOBALS['_dbta']) && sizeof($dbta) && (sizeof($GLOBALS['_dbta']) > sizeof($dbta)))
				{
					foreach($GLOBALS['_dbta'] as $tn => $td)
					{
						if(!isset($dbta[$tn]))
						{
							$ci='Table ['.$tn.'] not found';
							$cst .= '<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;<b>'.$tn.'</b>'.dbStatus(1, '^<b>'.$tn.'</b><hr size=1 /><br />'.$ci, '1 change');

							$fcta[$tn]=true;

							$tla=dbGetDefLine('T', $tn, $td);
							$fl='*T|'.$tla['line'].'|'.$tla['checksum'];

							$pa['filecontent'] .= $fl.$GLOBALS['nl_b'];
							$pa['tablearray'][$tn]='new';
							$pa['changelines']++;
						}
					}
				}

				if(strlen($ko))
				{
					$pa['status']=-1;
					$pa['message']=$ko.'!';
					$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
				}
				else if(strlen($cst))
					$pa['screencontent'] .= $cst.'<br />'.$GLOBALS['nl'];
				else if(!$fas)
					$pa['screencontent'] .= dbStatus(-1, 'No changes.').'<br />'.$GLOBALS['nl'];
			}

			// ...Primary keys
			// *T|abhaengigkeiten|204|DAT|1|1|mandanten_id:22:5:I8:1,projekt_a:2001:5:I8:1,projekt_b:2002:5:I8:1,abh_typ:2003:1:C2:1,tagesanzahl:921:5:I8:0,historie:1729:4:I2:0,transid:12:5:I8:0,aenderungs_id:84:5:I8:1,zeitstempel:19:10:C14:1|PK_530_204:mandanten_id,projekt_a,projekt_b,abh_typ:0|24297824e30041f6b3d48537aaa7a851
			if(!strlen($ko) && sizeof($fpka))
			{
				if(!$fas)
					$pa['screencontent'] .= '&nbsp;&nbsp;&nbsp;<b>&middot;</b>&nbsp;Checking primary keys:';
				$cst='';
				foreach($fpka as $rtn => $pka)
				{
					$c=0;
					$ci='';

					$pktdb=array();
					$pkcdb='';
					if(sizeof($pka['tpk']))
					{
						$pkta=explode(',', $pka['tpk']['columns']);
						foreach($pkta as $pktc)
						{
							$pktdb[trim($pktc)]=true;
							if(strlen($pkcdb))
								$pkcdb .= ',';
							$pkcdb .= trim($pktc);
						}
					}
//fxDebug($pktdb, 'PKTDB: '.$ltn);
					$pktfl=array();
					$pkcfl='';
					if(strlen($pka['fpk']))
					{
						$pkta=explode(':', $pka['fpk']);
						if(sizeof($pkta) == 3)
						{
							$pkta=explode(',', $pkta[1]);
							foreach($pkta as $pktc)
							{
								$pktfl[trim($pktc)]=true;
								if(strlen($pkcfl))
									$pkcfl .= ',';
								$pkcfl .= trim($pktc);
							}
						}
					}
//fxDebug($pktfl, 'PKTFL: '.$ltn);
					if(sizeof($pktdb) != sizeof($pktfl))
					{
						$c++;	// Number of primary key columns has changed
						$ci .= ', Number of primary key columns has changed from ['.sizeof($pktdb).' = ('.$pkcdb.')] to ['.sizeof($pktfl).' = ('.$pkcfl.')]';
					}
					if(sizeof($pktfl))
					{
						foreach($pktfl as $pktc => $pkd)
						{
							if(!sizeof($pktdb) || !isset($pktdb[$pktc]))
							{
								$c++;	// Primary key column is missing
								$ci .= ', Primary key column ['.$pktc.'] is missing in ('.$pkcfl.')';
							}
						}
					}

					if($c)
					{
						$gc++;
						$cpl='';
						if($c > 1)
							$cpl='s';
						$cst .= '<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;<b>'.$rtn.'</b>'.dbStatus(1, '^<b>'.$rtn.'</b><hr size=1 /><br />'.substr($ci,2), $c.' change'.$cpl);

						if(!isset($fcta[$rtn]))
						{
							$pa['filecontent']  .= trim($fa[$pka['fc']]).$GLOBALS['nl_b'];
							$pa['changelines']++;
						}

						if(!$create)
						{
							if(sizeof($pka['tpk']) && strlen($pka['tpk']['name']))
							{
								$sql="ALTER TABLE ";
								if($GLOBALS['_dbtype'] == 'sqlsrv')
									$sql .= "dbo.";
								$sql .= $rtn." DROP CONSTRAINT ".$pka['tpk']['name'];
								$sql .= $GLOBALS['fxpglobals']['dbparam']['dbcascade'];

								$pa['sqlarray'][]=$sql;
							}
							if(strlen($pka['pkn']))
							{
								$sql="ALTER TABLE ";
								if($GLOBALS['_dbtype'] == 'sqlsrv')
									$sql .= "dbo.";
								$sql .= $rtn." ADD ".$pka['pkn'];

								$pa['sqlarray'][]=$sql;
							}
						}
					}
				}

				if(strlen($cst))
					$pa['screencontent'] .= $cst.'<br />'.$GLOBALS['nl'];
				else if(!$fas)
					$pa['screencontent'] .= dbStatus(-1, 'No changes.').'<br />'.$GLOBALS['nl'];
			}

			// ...Recreate views
			if(!strlen($ko) && sizeof($fvwa))
			{
//fxDebug($fvwa,'$fvwa');
				foreach($fvwa as $vid => $va)
				{
					$otn=$va[0];
					if(isset($va[1]))
					{
						$ntn=$va[1];
						$sql=strtr($GLOBALS['_dbva'][$vid]['sql'], array(' '.$otn.' '=>' '.$ntn.' ', '('.$otn.' '=>'('.$ntn.' ', ' '.$otn.')'=>' '.$ntn.')'));
					}
					else
						$sql=$GLOBALS['_dbva'][$vid]['sql'];
					$pa['sqlarray'][]=dbCreateViewSQL($GLOBALS['_dbva'][$vid]['name'],$sql,false);
				}
			}

			if($pa['status'] >= 0)
			{
				$pa['status']=1;
				if($gc)
				{
					$gcpl='';
					if($GLOBALS['fxpglobals']['lang'] == 1)
					{
						if($gc > 1)
							$gcpl='en';
						$pa['message']=$gc.' Änderung'.$gcpl.' entdeckt.';
					}
					else
					{
						if($gc > 1)
							$gcpl='s';
						$pa['message']=$gc.' change'.$gcpl.' detected.';
					}
				}
				else if($GLOBALS['fxpglobals']['lang'] == 1)
					$pa['message']='Keine Änderungen.';
				else
					$pa['message']='No changes.';
			}
		}
		else
		{
			$pa['status']=-1;
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$pa['message']='Datei ist leer!';
			else
				$pa['message']='File is empty!';
			$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
		}
	}
	else
	{
		$pa['status']=-1;
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$pa['message']='Datei nicht gefunden!';
		else
			$pa['message']='File not found!';
		$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
	}

	return $pa;
}

/**
 * ???FUNCTION???
 *
 * @param various $fvwa - Mandatory parameter:                ???PARAMETER???
 * @param various $ttn  - Mandatory parameter:                ???PARAMETER???
 * @param string  $ltn  - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbCheckDropViews($fvwa, $ttn,$ltn='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dvsa=array();
	if(fxIsArray($GLOBALS['_dbva']))
	{
		foreach($GLOBALS['_dbva'] as $vid => $va)
		{
			if(!isset($fvwa[$vid]) && fxIsArray($va['tables']))
			{
				foreach($va['tables'] as $tn => $dummy)
				{
					if(($tn === $ttn) || (strlen($ltn) && ($tn === $ltn)))
					{
						$fvwa[$vid]=array($ttn,$ltn);
						$dvsa[$vid]=dbDropViewSQL($va['name']);
					}
				}
			}
		}
	}
//fxDebug($dvsa,'$dvsa: $ttn='.$ttn.', $ltn='.$ltn);

	return $dvsa;
}

/**
 * mode: -1=Create patch, 0=Execute update, 1=Execute patch
 *
 * @param various $vd   - Mandatory parameter:                     ???PARAMETER???
 * @param integer $mode - Optional parameter (default = 1):        ???PARAMETER???
 * @param boolean $fta  - Optional parameter (default = false):    ???PARAMETER???
 * @param array   $fa   - Optional parameter (default = array()):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbGetIndexDiff($vd, $mode=1, $fta=false, $fa=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pa=array(
		'status'		=> 0,
		'message'		=> '',
		'screencontent'	=> '',
		'filename'		=> '',
		'filelines'		=> 0,
		'rellines'		=> 0,
		'changelines'	=> 0,
		'filecontent'	=> '',
		'tablearray'	=> array(),
		'sqlarray'		=> array()
	);

	$create=false;
	if($mode < 0)
		$create=true;

	if($mode == 1)
	{
		$pa['filename']=$GLOBALS['datpath'].$vd.'_pat.txt';
		$ptype=' <i>(Patch)</i>';
		$vpcod='PATCH';
		$GLOBALS['_dbta']=array();
	}
	else
	{
		$pa['filename']=$GLOBALS['datpath'].$vd.'_vrs.txt';
		$ptype='';
		$vpcod='VERSION';
	}

	$vp=(int)substr($vd,0,3).'.'.(int)substr($vd,3,3).'.'.(int)substr($vd,6,3);

	// Reading table indexes
	if(($mode == -1) && fxIsArray($fta))
	{
		$pa['screencontent'] .= 'Adding all indexes for changed or new tables for version <b>'.$vp.'</b>'.dbStatus(1, sizeof($fta).' tables.').'<br />'.$GLOBALS['nl'];
		$pa['screencontent'] .= '&nbsp;&nbsp;&nbsp;<b>&middot;</b>&nbsp;Adding tables:';
		$tc=0;
		foreach($fta as $rtn => $ts)
		{
			if(!isset($GLOBALS['_dbta'][$rtn]))
				dbStructure($rtn);
			// Does the table have indexes?
			if(sizeof($GLOBALS['_dbta'][$rtn]['indexes']))
			{
				$tc++;
				$ic=0;
				$ci='';
				foreach($GLOBALS['_dbta'][$rtn]['indexes'] as $in => $id)
				{
					$tla=dbGetDefLine('I', $rtn, $GLOBALS['_dbta'][$rtn], $ic, $id);
					$pa['filecontent'] .= '*I|'.$tla['line'].'|'.$tla['checksum'].$GLOBALS['nl_b'];
					$pa['changelines']++;
					$ci .= ', '.$in;

					$ic++;
				}
				$ipl='';
				if($ic > 1)
					$ipl='es';
				$pa['screencontent'] .= '<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;<b>'.$rtn.'</b>'.dbStatus(1, '^<b>'.$rtn.'</b><hr size=1 /><br />'.substr($ci,2), $ic.' index'.$ipl);

				$pa['tablearray'][$rtn]=true;
			}
		}
		if(!$tc)
			$pa['screencontent'] .= dbStatus(-1, 'No tables added.');
		$pa['screencontent'] .= '<br />'.$GLOBALS['nl'];
	}

	$fas=false;
	if(fxIsArray($fa))
		$fas=true;

	// Reading table structure file
	if(!$fas)
		$pa['screencontent'] .= 'Reading file &quot;'.$pa['filename'].'&quot; for <b>Indexes</b>'.$ptype.' in version <b>'.$vp.'</b>';
	if($fas || file_exists($pa['filename']))
	{
		if(!$fas)
			$fa=fxLoad($pa['filename'], 1);
		if(fxIsArray($fa))
		{
			$pa['filelines']=sizeof($fa);
			$pa['screencontent'] .= dbStatus(1, 'Read '.$pa['filelines'].' lines.').'<br />'.$GLOBALS['nl'];

			$gc=0;

			$pa['screencontent'] .= '<!--#CHECK#-->';

			// ...Headline
			$ha=dbGetHeader($fa);
//fxDebug($ha,'$ha', 0);
			$pa['screencontent'] .= '&nbsp;&nbsp;&nbsp;<b>&middot;</b>&nbsp;Checking header';
			if(!strlen($ha['filename']) || !strlen($ha['version']) || !strlen($ha['versioncode']) || !strlen($ha['date']) || !strlen($ha['time']) || !strlen($ha['checksum']))
				$ko='TH.1 - Header parameter missing';
			if(!strlen($ko) && (strlen($ha['filename']) != 9))
				$ko='TH.2 - Wrong filename parameter length';
			if(!strlen($ko) && ((($mode != 1) && (strlen($ha['versioncode']) != 9)) || (($mode == 1) && (strlen($ha['versioncode']) != 13))))
				$ko='TH.3 - Wrong versioncode parameter length';
			if(!strlen($ko))
			{
				$hd=$vpcod.'-'.$ha['versioncode'].'-'.str_replace('-', '', $ha['date']).str_replace(':', '', $ha['time']);
				$h5=lineCode($hd, $vpcod);
				if($ha['checksum'] !== $h5)
					$ko='TH.4 - Checksum mismatch';
			}
			if(strlen($ko))
			{
				$pa['status']=-1;
				$pa['message']=$ko.'!';
				$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
			}
			else
				$pa['screencontent'] .= dbStatus(1).'<br />'.$GLOBALS['nl'];

			// ...Indexes
			// *I|abhaengigkeiten|204|IDX_530_204|mandanten_id,projekt_a,projekt_b,abh_typ|0|26c8afe0be8ac0a1d9b1b56e07fce19e
			if(!strlen($ko))
			{
				$pa['screencontent'] .= '&nbsp;&nbsp;&nbsp;<b>&middot;</b>&nbsp;Checking indexes:';
				$cst='';
				foreach($fa as $fc => $fl)
				{
					if($fc)
					{
						// Validity check
						$fl=trim($fl);
						$la=explode('|', $fl);
//fxDebug($la,'$la: '.fxHtmlEncode($fl));

						// Do we have a index definition [*I] line?
						if($la[0] === '*I')
						{
							$pa['rellines']++;
							// ...Line must contain exactly 7 data entries
							if(sizeof($la) != 7)
								$ko='IL'.$fc.'.1';
							else
							{
								foreach($la as $ln => $lv)
									$la[$ln]=trim($lv);
							}

							// ...1st entry must be index definition identifier [*I]
							if(!strlen($ko) && ($la[0] !== '*I'))
								$ko='IL'.$fc.'.2';

							// ...Checksum must match
							if(!strlen($ko))
							{
								$in=$la[3];
								$tl=substr($fl,3,-33);
								$t5=lineCode($tl, $in);
								if($la[6] !== $t5)
									$ko='IL'.$fc.'.3';
							}

							// Check changes
							if(!strlen($ko))
							{
								$rtn=$la[1];
//fxDebug($GLOBALS['_dbta'][$rtn]['indexes'],$rtn);
								// ...Format integer types: 2=table id + 5=clustered flag
								$la[2]=(int)$la[2];
								$la[5]=(int)$la[5];

								$c=0;
								$ci='';

								// Exists table and not already done?
								$tex=true;
								if(!isset($GLOBALS['_dbta'][$rtn]))
									dbStructure($rtn);
								if(!isset($GLOBALS['_dbta'][$rtn]))
								{
									$ren=false;
									foreach($GLOBALS['_dbta'] as $ttn => $tda)
									{
										if($tda['id'] == $la[2])
										{
											$rtn=$ttn;
											$ren=true;
											break;
										}
									}
									if(!$ren)
									{
										$c++;
										$ci .= ', Table ['.$rtn.'] not found';
										$tex=false;
									}
								}
								if($tex && !isset($pa['tablearray'][$rtn]))
								{
									// Does the table already have indexes?
									$iex=true;
									if(!sizeof($GLOBALS['_dbta'][$rtn]['indexes']))
									{
										$c++;
										$ci .= ', Table has no indexes -- New index is: ['.$in.': ('.$la[4].')]';
										$iex=false;

										if(!$create)
										{
											$sql=dbCreateIndexSQL($rtn,$la);
											if(strlen($sql))
												$pa['sqlarray'][]=$sql;
										}
									}

									// Check index columns
									if($iex)
									{
										$aex=false;

										// Index columns from file
										$cta=explode(',', $la[4]);
										$ca=array();
										foreach($cta as $cn)
											$ca[$cn]=true;
//fxDebug($ca,'$ca: '.$in);

										// Cycle through all table indexes
										$mif='';
										$tif='';
										foreach($GLOBALS['_dbta'][$rtn]['indexes'] as $tin => $tia)
										{
											if(strtoupper($tin) == strtoupper($in))	// Index with this name already exists
												$aex=true;

											$tcta=explode(',', $tia['columns']);
											$tca=array();
											foreach($tcta as $tcn)
												$tca[$tcn]=true;
//fxDebug($tca,'$tca: '.$tin);

											if(sizeof($ca) == sizeof($tca))
											{
												$cim=0;
												foreach($ca as $cn => $cdm)
												{
													if(isset($tca[$cn]))
														$cim++;
												}
												if($cim == sizeof($ca))
												{
													$mif=$tin.': ('.$tia['columns'].')';
													break;
												}
											}

											if(!strlen($mif))
												$tif .= ', '.$tin.': ('.$tia['columns'].')';
										}

										if(!strlen($mif))
										{
											$c++;
											$ci .= ', No match found for index ['.$in.': ('.$la[4].')] -- Indexes found are:'.substr($tif,1);

											if(!$create)
											{
												if($aex)
												{
													$sql=dbDropIndexSQL($rtn,$la);
													if(strlen($sql))
														$pa['sqlarray'][]=$sql;
												}
												$sql=dbCreateIndexSQL($rtn,$la);
												if(strlen($sql))
													$pa['sqlarray'][]=$sql;
											}
										}
									}
								}

								if($c)
								{
									$gc++;
									$cpl='';
									if($c > 1)
										$cpl='s';
									$cst .= '<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;'.$rtn.': <b>'.$in.'</b>'.dbStatus(1, '^'.$rtn.': <b>'.$in.'</b><hr size=1 /><br />'.substr($ci,2), $c.' change'.$cpl);

									if(!isset($fcta[$rtn]))
									{
										$pa['filecontent']  .= $fl.$GLOBALS['nl_b'];
										$pa['changelines']++;
									}
								}
							}
						}
					}

					if(strlen($ko))
						break;
				}

				if(strlen($ko))
				{
					$pa['status']=-1;
					$pa['message']=$ko.'!';
					$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
				}
				else if(strlen($cst))
					$pa['screencontent'] .= $cst.'<br />'.$GLOBALS['nl'];
				else
					$pa['screencontent'] .= dbStatus(-1, 'No changes.').'<br />'.$GLOBALS['nl'];
			}

			if($pa['status'] >= 0)
			{
				$pa['status']=1;
				if($gc)
				{
					$gcpl='';
					if($GLOBALS['fxpglobals']['lang'] == 1)
					{
						if($gc > 1)
							$gcpl='en';
						$pa['message']=$gc.' Änderung'.$gcpl.' entdeckt.';
					}
					else
					{
						if($gc > 1)
							$gcpl='s';
						$pa['message']=$gc.' change'.$gcpl.' detected.';
					}
				}
				else if($GLOBALS['fxpglobals']['lang'] == 1)
					$pa['message']='Keine Änderungen.';
				else
					$pa['message']='No changes.';
			}
		}
		else
		{
			$pa['status']=-1;
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$pa['message']='Datei ist leer!';
			else
				$pa['message']='File is empty!';
			$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
		}
	}
	else
	{
		$pa['status']=-1;
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$pa['message']='Datei nicht gefunden!';
		else
			$pa['message']='File not found!';
		$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
	}

	return $pa;
}

/**
 * mode: -1=Create patch, 0=Execute update, 1=Execute patch
 *
 * @param various $vd   - Mandatory parameter:                     ???PARAMETER???
 * @param integer $mode - Optional parameter (default = 1):        ???PARAMETER???
 * @param array   $fa   - Optional parameter (default = array()):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbGetDataDiff($vd, $mode=1, $fa=array())
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dra=dbGetDefRepl(false);

	$pa=array(
		'status'		=> 0,
		'message'		=> '',
		'screencontent'	=> '',
		'filename'		=> '',
		'filelines'		=> 0,
		'rellines'		=> 0,
		'changelines'	=> 0,
		'filecontent'	=> '',
		'sqlarray'		=> array()
	);

	$create=false;
	if($mode < 0)
		$create=true;

	if($mode == 1)
	{
		$pa['filename']=$GLOBALS['datpath'].$vd.'_pat.txt';
		$ptype=' <i>(Patch)</i>';
		$vpcod='PATCH';
		$GLOBALS['_dbta']=array();
	}
	else
	{
		$pa['filename']=$GLOBALS['datpath'].$vd.'_vrs.txt';
		$ptype='';
		$vpcod='VERSION';
	}

	$vp=(int)substr($vd,0,3).'.'.(int)substr($vd,3,3).'.'.(int)substr($vd,6,3);

	$fas=false;
	if(fxIsArray($fa))
		$fas=true;

	// Reading index file
	if(!$fas)
		$pa['screencontent'] .= 'Reading file &quot;'.$pa['filename'].'&quot; for <b>Indexes</b>'.$ptype.' in version <b>'.$vp.'</b>';
	if($fas || file_exists($pa['filename']))
	{
		if(!$fas)
			$fa=fxLoad($pa['filename'], 1);
		if(fxIsArray($fa))
		{
			$pa['filelines']=sizeof($fa);
			if(!$fas)
				$pa['screencontent'] .= dbStatus(1, 'Read '.$pa['filelines'].' lines.').'<br />'.$GLOBALS['nl'];

			$ptra=array('"'=>'&quot;');
			$gc=0;

			$pa['screencontent'] .= '<!--#CHECK#-->';

			// ...Headline
			$ha=dbGetHeader($fa);
//fxDebug($ha,'$ha', 0);
			$pa['screencontent'] .= '&nbsp;&nbsp;&nbsp;<b>&middot;</b>&nbsp;Checking header';
			if(!strlen($ha['filename']) || !strlen($ha['version']) || !strlen($ha['versioncode']) || !strlen($ha['date']) || !strlen($ha['time']) || !strlen($ha['checksum']))
				$ko='TH.1 - Header parameter missing';
			if(!strlen($ko) && (strlen($ha['filename']) != 9))
				$ko='TH.2 - Wrong filename parameter length';
			if(!strlen($ko) && ((($mode != 1) && (strlen($ha['versioncode']) != 9)) || (($mode == 1) && (strlen($ha['versioncode']) != 13))))
				$ko='TH.3 - Wrong versioncode parameter length';
			if(!strlen($ko))
			{
				$hd=$vpcod.'-'.$ha['versioncode'].'-'.str_replace('-', '', $ha['date']).str_replace(':', '', $ha['time']);
				$h5=lineCode($hd, $vpcod);
				if($ha['checksum'] !== $h5)
					$ko='TH.4 - Checksum mismatch';
			}
			if(strlen($ko))
			{
				$pa['status']=-1;
				$pa['message']=$ko.'!';
				$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
			}
			else
				$pa['screencontent'] .= dbStatus(1).'<br />'.$GLOBALS['nl'];

			// ...DATA
			if(!strlen($ko))
			{
				$pa['screencontent'] .= '&nbsp;&nbsp;&nbsp;<b>&middot;</b>&nbsp;Checking data:';
				$cst='';
				$c=0;
				$ci='';
				$tia=array();
				$fca=array();
				foreach($fa as $fc => $fl)
				{
					if($fc)
					{
						// Validity check
						$fl=trim($fl);
						$la=explode('|', $fl);
//fxDebug($la,'$la: '.fxHtmlEncode($fl));

						// Do we have a table definition [*DT] or data [*D{1...}] line?
						if($la[0] === '*DT')
						{
							$pa['rellines']++;
							// Handle remaining actions for previous table
							if(sizeof($tia))
							{
								$gc += $c;

								// Unprocessed dataentries?
								if(sizeof($tia['datarray']))
								{
									foreach($tia['datarray'] as $dpk => $dvl)
									{
										if($create)
										{
											$gc++;
											$c++;
											$ci .= '|Dataset with primary key ['.$dpk.'] not processed - Insert dataset [<b title="'.strtr($dvl,$ptra).'" style="cursor:help;">'.$dpk.'</b>]';

											$fca[] = $dvl;
										}
										else if(isset($GLOBALS['fxupdate']))
										{
											$gc++;
											$c++;
											$ci .= '|Dataset with primary key ['.$dpk.'] found - Delete dataset [<b title="'.strtr($dvl,$ptra).'" style="cursor:help;">'.$dpk.'</b>]';

											$where='';
											$tda=explode(',', $dpk);
											$tdpc=0;
											foreach($tia['datpkcolarray'] as $cn => $cp)
											{
												if(strlen($where))
													$where .= " AND ";
												$where .= $cn."=".strtr($tda[$tdpc],$dra);
												$tdpc++;
											}
											$sql="DELETE FROM ".$tia['name']." WHERE ".$where;
											$pa['sqlarray'][] = $sql;
										}
									}
								}

								// Display messages?
								if($c)
								{
									$cpl='';
									if($c > 1)
										$cpl='s';
									$cst .= '<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;'.$tia['name'].': '.dbStatus(1, '^<b>'.$tia['name'].'</b><hr size=1 /><br />'.str_replace('|', '<br />', substr($ci,1)), $c.' change'.$cpl);

									if(sizeof($fca) && $create)
									{
//fxDebug($tia,'$tia');
//fxDebug($fca,'$fca: '.$tia['name']);
										// ...Table header
										$tla=dbGetDefLine('D', $tia['name'], $tia, -1, $fca);
										$pa['filecontent'] .= '*DT|'.$tla['line'].'|'.$tla['checksum'].$GLOBALS['nl_b'];

										// ...Table data
										foreach($fca as $fcln => $fcla)
										{
											$tla=dbGetDefLine('D', $tia['name'], $tia, $fcln+1, $fcla);
											$pa['filecontent'] .= '*D'.($fcln+1).'|'.$tla['line'].'|'.$tla['checksum'].$GLOBALS['nl_b'];
											$pa['changelines']++;
										}
									}
								}
							}

							$c=0;
							$ci='';
							$tia=array();
							$fca=array();

							// ...Line must contain exactly 6 data entries
							// *DT|d_dok_elt_art|94|11|mandanten_id,elter_art_id,elter_art_name,transid,aenderungs_id,zeitstempel|e6cc9075f2be579c6217ca68f7145dfe
							if(sizeof($la) != 6)
								$ko='DL'.$fc.'.T1';
							else
							{
								foreach($la as $ln => $lv)
									$la[$ln]=trim($lv);
							}

							// ...Checksum must match
							if(!strlen($ko))
							{
								$tl=substr($fl,4,-33);
								$t5=lineCode($tl, $la[1].'-'.$la[3]);
								if($la[5] !== $t5)
									$ko='DL'.$fc.'.T2';
							}

							// ...Table must exist (TODO: What if table has been deleted?)
							if(!strlen($ko))
							{
	 							if(!isset($GLOBALS['_dbta'][$la[1]]))
									dbStructure($la[1]);
	 							if(!isset($GLOBALS['_dbta'][$la[1]]))
									$ko='DL'.$fc.'.T3';
							}

							// ...Create table definition array
							if(!strlen($ko))
							{
								$tia=array('name'=>$la[1], 'id'=>(int)$la[2], 'client'=>false, 'sql'=>"", 'lines'=>(int)$la[3], 'pkcolumns'=>"", 'pkcolarray'=>array(), 'columns'=>$la[4], 'colsize'=>0, 'colarray'=>array(), 'colnarray'=>array(), 'datpkcolumns'=>"", 'datpkcolarray'=>array(), 'datcolumns'=>"", 'datcolsize'=>0, 'datcolarray'=>array(), 'datcolnarray'=>array(), 'datarray'=>array());
								$tpa=array();
								if(fxIsArray($GLOBALS['_dbta'][$tia['name']]['primarykey']))
								{
									$cta=explode(',', $GLOBALS['_dbta'][$tia['name']]['primarykey']['columns']);
									foreach($cta as $cn)
										$tpa[$cn]=true;
								}
								$tia['colarray']=explode(',', $la[4]);
								foreach($tia['colarray'] as $cp => $cn)
								{
									$tia['colnarray'][$cn]=$cp;
									$tia['colsize']++;
									if($cn == 'mandanten_id')
										$tia['client']=true;
									if(isset($tpa[$cn]))
									{
										$tia['pkcolarray'][$cn]=$cp;

										if(strlen($tia['pkcolumns']))
											$tia['pkcolumns'] .= ',';
										$tia['pkcolumns'] .= $cn;
									}
								}

								// ...Primary key columns must match (TODO: What if primary key has changed?)
								if(sizeof($tpa) != sizeof($tia['pkcolarray']))
								{
									$ko='DL'.$fc.'.T4';
									break;
								}

								// Read data
								if(!strlen($ko))
								{
									if(!$create)
										$pa['sqlarray'][] = '*'.$tia['name'];

//function dbGetDataSQL($tablename, $client_table=false, $columns='*', $pk_columns='')
									if($create)
										$tia['sql']=dbGetDataSQL($tia['name'], $tia['client'], '*', $tia['pkcolumns']);
									else
									{
										$sc='';
										if(fxIsArray($GLOBALS['_dbta'][$tia['name']]) && fxIsArray($GLOBALS['_dbta'][$tia['name']]['columns']))
										{
											foreach($tia['colnarray'] as $cname => $cpos)
											{
												if(fxIsArray($GLOBALS['_dbta'][$tia['name']]['columns'][$cname]))
												{
													if(strlen($sc))
														$sc .= ',';
													$sc .= $cname;
												}
											}
										}
										if(!strlen($sc))
											$sc='*';
										$tia['sql']=dbGetDataSQL($tia['name'], $tia['client'], $sc, $tia['pkcolumns']);
									}
									$tca=db_values($tia['sql'],'*');
									if(fxIsArray($tca))
									{
										// ...Table header and format
										$dcp=0;
										foreach($tca as $tk => $ta)
										{
											foreach($ta as $cn => $cv)
											{
												if(!$tk)
												{
													if(strlen($tia['datcolumns']))
														$tia['datcolumns'] .= ',';
													$tia['datcolumns'] .= $cn;
													$tia['datcolsize']++;
													$tia['datcolarray'][$dcp]=$cn;
													if(!$create || isset($tia['colnarray'][$cn]))
														$tia['datcolnarray'][$cn]=$tia['colnarray'][$cn];
													else
														$tia['datcolnarray'][$cn]=-1;
													$dcp++;
												}

												if(strlen($cv) && fxIsArray($GLOBALS['_dbta'][$la[1]]['columns'][$cn]))
												{
													$ctype=substr($GLOBALS['_dbta'][$la[1]]['columns'][$cn]['type'],0,1);
													if(($ctype == 'I') || ($ctype == 'S'))
														$tca[$tk][$cn]=(int)$cv;
													else if($ctype == 'D')
														$tca[$tk][$cn]=(float)$cv;
													else
														$tca[$tk][$cn]=(string)$cv;
												}
											}
										}

										// ...Column sizes must match for updates
										if(!$create && ($tia['colsize'] != $tia['datcolsize']))
											$ko='DL'.$fc.'.T5';

										// ...Table data
										if(!strlen($ko))
										{
											$dc=1;
											foreach($tca as $da)
											{
												$tla=dbGetDefLine('D', $tia['name'], $tia, $dc, $da);
												$tle=explode(',', $tla['line']);
												$dpk='';
												$dln='';
												foreach($tia['datcolarray'] as $cp => $cn)
												{
													if(isset($tia['pkcolarray'][$cn]))
													{
														if(strlen($dpk))
															$dpk .= ',';
														$dpk .= $tle[$cp];

														if(!isset($tia['datpkcolarray'][$cn]))
														{
															$tia['datpkcolarray'][$cn]=$cp;

															if(strlen($tia['datpkcolumns']))
																$tia['datpkcolumns'] .= ',';
															$tia['datpkcolumns'] .= $cn;
														}
													}
													if(strlen($dln))
														$dln .= ',';
													$dln .= $tle[$cp];
												}
												$tia['datarray'][$dpk]=$dln;

												$dc++;
											}
										}
									}
								}
							}
						}
						else if(substr($la[0],0,2) === '*D')
						{
							$pa['rellines']++;
							// ...Line must contain exactly 3 data entries
							// *D1|0,1,961,0,0,'20101119110000'|7c1c9ef57875cebd283e9aec6514e086
							if(sizeof($la) != 3)
								$ko='DL'.$fc.'.D1';
							else
							{
								foreach($la as $ln => $lv)
									$la[$ln]=trim($lv);
							}

							// ...Table definition must be set
							if(!strlen($ko) && !sizeof($tia))
								$ko='DL'.$fc.'.D2';

							// ...Checksum must match
							if(!strlen($ko))
							{
								$dc=(int)substr($la[0],2);
								$tl=substr($fl,strlen($la[0])+1,-33);
								$t5=lineCode($tl, $tia['name'].'-'.$dc);
								if($la[2] !== $t5)
									$ko='DL'.$fc.'.D3';
							}

							// ...Column numbers must match, except for delete
							$del=false;
							if(!strlen($ko))
							{
								if(substr($la[1],0,1) == '~')	// Delete
								{
									$del=true;
									$la[1]=substr($la[1],1);
								}
								else
								{
									$cc=1+substr_count($la[1],',');
									if($cc != $tia['colsize'])
										$ko='DL'.$fc.'.D4';
								}
							}

							// ...Check changes
							if(!strlen($ko))
							{
								$dln=(int)substr($la[0],2);
								$lcf=false;
								$lcd='';

								// Get primary key data
								$tle=explode(',', $la[1]);
								$dpk='';
								foreach($tia['colarray'] as $cc => $cn)
								{
									if(isset($tia['pkcolarray'][$cn]))
									{
										if(strlen($dpk))
											$dpk .= ',';
										$dpk .= $tle[$cc];
									}
								}

								if($create && isset($tia['datarray'][$dpk]))
									$line=$tia['datarray'][$dpk];
								else
									$line=$la[1];

								if(!sizeof($tia['datarray']))
								{
									if($create)
									{
										$lcf=true;
										$c++;
										$lcd='~'.$line;
										$ci .= '|Table is empty - Delete dataset <b title="'.strtr($line,$ptra).'" style="cursor:help;">#'.$dln.'</b>';
									}
									else if(!$del)
									{
										$lcf=true;
										$c++;
										$lcd=$line;
										$ci .= '|Table is empty - Insert dataset <b title="'.strtr($line,$ptra).'" style="cursor:help;">#'.$dln.'</b>';

										$sql="INSERT INTO ".$tia['name']." (".dbReplaceCommaNull($tia['columns']).") VALUES (".strtr(dbReplaceCommaNull($line),$dra).")";
										$pa['sqlarray'][] = $sql;
									}
								}
								else
								{
									// Check if a dataset with this primary key combination already exists
									// ...No
									if(!isset($tia['datarray'][$dpk]))
									{
										if($create)
										{
											$lcf=true;
											$c++;
											$lcd='~'.$line;
											$ci .= '|Dataset with primary key ['.$dpk.'] not found - Delete dataset <b title="'.strtr($line,$ptra).'" style="cursor:help;">#'.$dln.'</b>';
										}
										else if(!$del)
										{
											$lcf=true;
											$c++;
											$lcd=$line;
											$ci .= '|Dataset with primary key ['.$dpk.'] not found - Insert dataset <b title="'.strtr($line,$ptra).'" style="cursor:help;">#'.$dln.'</b>';

											$sql="INSERT INTO ".$tia['name']." (".dbReplaceCommaNull($tia['columns']).") VALUES (".strtr(dbReplaceCommaNull($line),$dra).")";
											$pa['sqlarray'][] = $sql;
										}
									}
									// ...Yes
									else
									{
										// ...Check for changed columns only if there is any difference
										if(!$create && $del)
										{
											$lcf=true;
											$c++;
											$lcd=$line;
											$ci .= '|Dataset with primary key ['.$dpk.'] found - Delete dataset <b title="'.strtr($line,$ptra).'" style="cursor:help;">#'.$dln.'</b>';

											$where='';
											$tda=explode(',', $tia['datarray'][$dpk]);
											foreach($tia['colarray'] as $cc => $cn)
											{
												if(isset($tia['pkcolarray'][$cn]))
												{
													if(strlen($where))
														$where .= " AND ";
													$where .= $cn."=".strtr($tda[$cc],$dra);
												}
											}
											$sql="DELETE FROM ".$tia['name']." WHERE ".$where;
											$pa['sqlarray'][] = $sql;
										}
										else if($tia['datarray'][$dpk] != $la[1])
										{
											$cci='';
											$set='';
											$sad='';
											$tda=explode(',', $tia['datarray'][$dpk]);
											foreach($tia['datcolarray'] as $cc => $cn)
											{
												$oc=$tia['datcolnarray'][$cn];
												if($oc >= 0)	// Old value exists?
													$lcv=$tle[$oc];
												else
													$lcv='';
												$dcv=$tda[$cc];
												$ccc=true;
												// Ignore primary key, trans id, change id and timestamp columns
												if(isset($tia['pkcolarray'][$cn]) || ($cn == 'transid') || ($cn == 'aenderungs_id') || ($cn == 'zeitstempel'))
													$ccc=false;
												// Ignore version settings in wertetab (except for installation or update)
												if($ccc && $mode && ($tia['name'] == 'wertetab') && ($cn == 'tabwert') && (($tle[$tia['colnarray']['wertetabid']] == '10') || ($tle[$tia['colnarray']['wertetabid']] == '11') || ($tle[$tia['colnarray']['wertetabid']] == '12')))
													$ccc=false;
												// Ignore dms path in settings
												if($ccc && ($tia['name'] == 's_einstellungswerte') && ($cn == 'einstellungswert') && ($tle[$tia['colnarray']['einstellungswerte_id']] == '18'))
													$ccc=false;
												if($ccc)
												{
													// Adjust mandatory field in maskenfelder
													if(($tia['name'] == 'maskenfelder') && ($cn == 'muss'))
													{
														if($lcv)
															$lcv='1';
														else
															$lcv='0';
														if($dcv)
															$dcv='1';
														else
															$dcv='0';
													}
													$ccc=false;
													if($lcv != $dcv)
													{
														if(!$create)
														{
															$cci .= ', Column &quot;<b title="From ['.strtr($dcv,$ptra).'] to ['.strtr($lcv,$ptra).']" style="cursor:help;">'.$cn.'</b>&quot;';
															if(strlen($set))
																$set .= ", ";
															$set .= $cn."=";
															if(strlen($lcv))
																$set .= strtr($lcv,$dra);
															else
																$set .= "NULL";
														}
														else
														{
															$cci .= ', Column &quot;<b title="From ['.strtr($lcv,$ptra).'] to ['.strtr($dcv,$ptra).']" style="cursor:help;">'.$cn.'</b>&quot;';
														}
													}
												}
												else if(!$create && (($cn == 'transid') || ($cn == 'aenderungs_id') || ($cn == 'zeitstempel')))
												{
													if($oc >= 0)	// Old value exists?
														$lcv=$tle[$oc];
													else
														$lcv='';
													$sad .= ", ".$cn."=";
													if(strlen($lcv))
														$sad .= strtr($lcv,$dra);
													else
														$sad .= "NULL";
												}
											}
											if(strlen($cci))
											{
												$lcf=true;
												$c++;
												$ci .= '|Columns in dataset with primary key ['.$dpk.'] have changed:'.substr($cci,1);
												$lcd=$line;
												if(!$create && strlen($set))
												{
													$where='';
													foreach($tia['colarray'] as $cc => $cn)
													{
														if(isset($tia['datpkcolarray'][$cn]))
														{
															$dcc=$tia['datpkcolarray'][$cn];
															if(strlen($where))
																$where .= " AND ";
															$where .= $cn."=".strtr($tda[$dcc],$dra);
														}
													}
													$sql="UPDATE ".$tia['name']." SET ".$set.$sad." WHERE ".$where;
													$pa['sqlarray'][] = $sql;
												}
											}
										}
										unset($tia['datarray'][$dpk]);
									}
								}

								if($lcf && strlen($lcd))
									$fca[] = $lcd;
							}
						}
					}

					if(strlen($ko))
						break;
				}

				// Handle remaining actions for last table
				if(sizeof($tia))
				{
					$gc += $c;

					// Unprocessed dataentries?
					if(sizeof($tia['datarray']))
					{
						foreach($tia['datarray'] as $dpk => $dvl)
						{
							if($create)
							{
								$gc++;
								$c++;
								$ci .= '|Dataset with primary key ['.$dpk.'] not processed - Insert dataset [<b title="'.strtr($dvl,$ptra).'" style="cursor:help;">'.$dpk.'</b>]';

								$fca[] = $dvl;
							}
							else if(isset($GLOBALS['fxupdate']))
							{
								$gc++;
								$c++;
								$ci .= '|Dataset with primary key ['.$dpk.'] found - Delete dataset [<b title="'.strtr($dvl,$ptra).'" style="cursor:help;">'.$dpk.'</b>]';

								$where='';
								$tda=explode(',', $dpk);
								$tdpc=0;
								foreach($tia['datpkcolarray'] as $cn => $cp)
								{
									if(strlen($where))
										$where .= " AND ";
									$where .= $cn."=".strtr($tda[$tdpc],$dra);
									$tdpc++;
								}
								$sql="DELETE FROM ".$tia['name']." WHERE ".$where;
								$pa['sqlarray'][] = $sql;
							}
						}
					}

					// Display messages?
					if($c)
					{
						$cpl='';
						if($c > 1)
							$cpl='s';
						$cst .= '<br />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;-&nbsp;'.$tia['name'].': '.dbStatus(1, '^<b>'.$tia['name'].'</b><hr size=1 /><br />'.str_replace('|', '<br />', substr($ci,1)), $c.' change'.$cpl);

						if(sizeof($fca) && $create)
						{
//fxDebug($tia,'$tia');
//fxDebug($fca,'$fca: '.$tia['name']);
							// ...Table header
							$tla=dbGetDefLine('D', $tia['name'], $tia, -1, $fca);
							$pa['filecontent'] .= '*DT|'.$tla['line'].'|'.$tla['checksum'].$GLOBALS['nl_b'];

							// ...Table data
							foreach($fca as $fcln => $fcla)
							{
								$tla=dbGetDefLine('D', $tia['name'], $tia, $fcln+1, $fcla);
								$pa['filecontent'] .= '*D'.($fcln+1).'|'.$tla['line'].'|'.$tla['checksum'].$GLOBALS['nl_b'];
								$pa['changelines']++;
							}
						}
					}
				}

				if(strlen($ko))
				{
					$pa['status']=-1;
					$pa['message']=$ko.'!';
					$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
				}
				else if(strlen($cst))
					$pa['screencontent'] .= $cst.'<br />'.$GLOBALS['nl'];
				else
					$pa['screencontent'] .= dbStatus(-1, 'No changes.').'<br />'.$GLOBALS['nl'];
			}

			if($pa['status'] >= 0)
			{
				$pa['status']=1;
				if($gc)
				{
					$gcpl='';
					if($GLOBALS['fxpglobals']['lang'] == 1)
					{
						if($gc > 1)
							$gcpl='en';
						$pa['message']=$gc.' Änderung'.$gcpl.' entdeckt.';
					}
					else
					{
						if($gc > 1)
							$gcpl='s';
						$pa['message']=$gc.' change'.$gcpl.' detected.';
					}
				}
				else if($GLOBALS['fxpglobals']['lang'] == 1)
					$pa['message']='Keine Änderungen.';
				else
					$pa['message']='No changes.';
			}
		}
		else
		{
			$pa['status']=-1;
			if($GLOBALS['fxpglobals']['lang'] == 1)
				$pa['message']='Datei ist leer!';
			else
				$pa['message']='File is empty!';
			$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
		}
	}
	else
	{
		$pa['status']=-1;
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$pa['message']='Datei nicht gefunden!';
		else
			$pa['message']='File not found!';
		$pa['screencontent'] .= dbStatus(0, $pa['message']).'<br />'.$GLOBALS['nl'];
	}

	return $pa;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $execute - Optional parameter (default = true):  ???PARAMETER???
 * @param boolean $display - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbExecutePatch($execute=true, $display=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//$GLOBALS['force_fxDebug']=true;
	$psc=0;
	$perr=false;
	$sret=false;
	$emsg='';

	plog(-1, '', 0, __LINE__, __FILE__);
	$GLOBALS['swritelog']='';

	// Check if patch file exists
	$vd=$GLOBALS['fxpglobals']['dbparam']['versionscode_db'];
	$pfn=$GLOBALS['datpath'].$vd.'_pat.txt';
	$pfe=file_exists($pfn);
	// ...No, return without errors because there is nothing to do
	if(!$pfe)
		return false;

	// Get patch number
	$patchversion=0;
	$patchdate='';
	$handle=fopen($pfn,'r');
	if($handle)
	{
		// fx-project: PATCH 005003000-001-20151006112823|c56ff071261aa2caa4ccc419827888d3
		$patchheader=fread($handle,82);
		fclose($handle);

		if(strlen($patchheader))
		{
			$patchversion=(int)substr($patchheader,31,3);
			$patchdate=substr($patchheader,35,14);
		}
	}
	if($display)
		echo('<b class="s4">Patch #'.$patchversion.'</b>&nbsp;&nbsp;&nbsp;<i class="grey">(created on '.$patchdate.')</i><br />'.$GLOBALS['nl']);
	// ...No file patch version or already patched version greater than file patch version, return without errors because there is nothing to do
	$pvs=substr('000'.$patchversion,-3);
	if(!$patchversion || ($GLOBALS['fxpglobals']['dbparam']['version_patch'] > $patchversion))
	{
		// Rename patch file, so that it will not be executed again
		if($execute)
		{
			$pnfn=$GLOBALS['datpath'].$vd.'_pat_'.$GLOBALS['datetime'].'_'.$pvs.'-out.txt';
			@chmod($pnfn,0777);
			@unlink($pnfn);
			@rename($pfn,$pnfn);
		}
		return false;
	}

	// Create patch active file
	$pfa='';
	if($execute)
	{
		$pfa=$GLOBALS['datpath'].$vd.'_pat_active.txt';
		fxSave($pfa, $GLOBALS['datetime']);

		$GLOBALS['swritelog']='pat_'.$vd.'-'.$pvs.'_'.$GLOBALS['datetime'];
	}

	$rb=false;

	// Get table and primarykey differences
	if(!$perr)
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$t='Patch-Tabellenänderungen ermitteln';
		else
			$t='Investigating patch table changes';
		plog(2, $t.'...', 6, __LINE__, __FILE__);
		db_slog('*Investigating patch table changes', __LINE__, __FILE__);
		sleep(1);

		$pa=dbGetTablePKDiff($vd,false);
		db_slog('Done.', __LINE__, __FILE__);
		if($display)
			echo($pa['screencontent']);

		$sec=0;
		if($pa['status'] < 0)
		{
			$sret=true;
			$emsg=$pa['message'];
		}
		else
		{
			foreach($pa['sqlarray'] as $sql)
			{
				if(substr($sql,0,1) != '*')
					$sec++;
			}
		}

		if(!$sret)
		{
			db_slog('*Executing patch table changes', __LINE__, __FILE__);
			// ...Make changes
			if($sec)
			{
				if($GLOBALS['fxpglobals']['lang'] == 1)
					$t='Patch-Tabellenänderungen ausführen';
				else
					$t='Executing patch table changes';
				plog(6, $t.'...', 6, __LINE__, __FILE__);
				sleep(1);

//fxDebug($pa['sqlarray'],'dbGetDataDiff: $pa[\'sqlarray\']');
				if($display)
					echo('<br /><hr sizer="1" /><br /><b>SQL\'s</b> <i class="grey">(Transaction START)</i><br /><br />'.$GLOBALS['nl']);

				$dbc=0;
				$po=12;
				$err=false;
				if(!$rb)
				{
					db_rollback('start');
					$rb=true;
				}
				foreach($pa['sqlarray'] as $sql)
				{
					$pp=min(25, 12+ceil(13*$dbc/$sec));
					if($display)
						echo('<pre style="width:99%;white-space:normal;border:1px solid #cccccc;margin:0;margin-bottom:2px;padding:4px;border-radius:3px;"><b class="darkgrey">'.($dbc+1).' = '.$pp.'%</b> '.fxHtmlEncode($sql).'</pre>'.$GLOBALS['nl']);

					if(substr($sql,0,1) == '*')	// New Table
					{
						if($dbc && $execute)
						{
							if($rb)
								db_rollback('ok');
							db_rollback('start');
							$rb=true;
						}
						plog($pp, $t.'...', $pp, __LINE__, __FILE__);
					}
					else
					{
						$success=dbExecuteQuery($sql);
						if(!$success)
						{
							db_slog('ERROR! '.$sql, __LINE__, __FILE__);
							$err=true;
							break;
						}
						else
						{
							$dbc++;
							$psc++;
							if($pp != $po)
							{
								$po=$pp;
								plog($pp, $t.'...', $pp, __LINE__, __FILE__);
							}
						}
					}
				}
				if($err)
				{
					$perr=true;
					if($display)
						echo('<br /> <b class="s4">&rarr;</b> <b class="red">ERROR</b> <i class="grey">(Transaction ROLLBACK)</i><br />'.$GLOBALS['nl']);
					if($rb)
					{
						db_rollback('rollback');
						$rb=false;
					}
					$sret=true;
					$emsg='SQL T'.$dbc;
				}
				else if($execute)
				{
					if($display)
						echo('<br /> <b class="s4">&rarr;</b> <b class="green">SUCCESS</b> '.$dbc.' sql\'s executed. <i class="grey">(Transaction OK)</i><br />'.$GLOBALS['nl']);
					if($rb)
					{
						db_rollback('ok');
						$rb=false;
					}
					db_slog('SUCCESS: '.$dbc.' sql\'s executed.', __LINE__, __FILE__);
				}
				else
				{
					if($display)
						echo('<br /> <b class="s4">&rarr;</b> <b class="green">SUCCESS</b> '.$dbc.' sql\'s executed. <i class="grey">(Testcase - Transaction OK)</i><br />'.$GLOBALS['nl']);
				}
			}
			else
				db_slog('No changes found.', __LINE__, __FILE__);
		}
	}

	// Get index differences
	if(!$sret && !$perr)
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$t='Patch-Indexänderungen ermitteln';
		else
			$t='Investigating patch index changes';
		plog(25, $t.'...', 28, __LINE__, __FILE__);
		db_slog('*Investigating patch index changes', __LINE__, __FILE__);
		sleep(1);

		$pa=dbGetIndexDiff($vd);
		db_slog('Done.', __LINE__, __FILE__);
		if($display)
		{
			echo('<br /><hr sizer="1" />'.$GLOBALS['nl']);
			echo($pa['screencontent']);
		}

		$sec=0;
		if($pa['status'] < 0)
		{
			$sret=true;
			$emsg=$pa['message'];
		}
		else
		{
			foreach($pa['sqlarray'] as $sql)
			{
				if(substr($sql,0,1) != '*')
					$sec++;
			}
		}

		if(!$sret)
		{
			db_slog('*Executing patch index changes', __LINE__, __FILE__);
			// ...Make changes
			if($sec)
			{
//fxDebug($pa['sqlarray'],'dbGetDataDiff: $pa[\'sqlarray\']');
				if($display)
					echo('<br /><hr sizer="1" /><br /><b>SQL\'s</b> <i class="grey">(Transaction START)</i><br /><br />'.$GLOBALS['nl']);

				if($GLOBALS['fxpglobals']['lang'] == 1)
					$t='Patch-Indexänderungen ausführen';
				else
					$t='Executing patch index changes';
				plog(28, $t.'...', 28, __LINE__, __FILE__);
				sleep(1);

				$dbc=0;
				$err=false;
				if(!$rb)
				{
					db_rollback('start');
					$rb=true;
				}
				foreach($pa['sqlarray'] as $sql)
				{
					$pp=min(35, 28+ceil(7*$dbc/$sec));
					if($display)
						echo('<pre style="width:99%;white-space:normal;border:1px solid #cccccc;margin:0;margin-bottom:2px;padding:4px;border-radius:3px;"><b class="darkgrey">'.($dbc+1).' = '.$pp.'%</b> '.fxHtmlEncode($sql).'</pre>'.$GLOBALS['nl']);

					if(substr($sql,0,1) == '*')	// New Table
					{
						if($dbc && $execute)
						{
							if($rb)
								db_rollback('ok');
							db_rollback('start');
							$rb=true;
						}
						plog($pp, $t.'...', $pp, __LINE__, __FILE__);
					}
					else
					{
						$success=dbExecuteQuery($sql);
						if(!$success)
						{
							db_slog('ERROR! '.$sql, __LINE__, __FILE__);
							$err=true;
							break;
						}
						else
						{
							$dbc++;
							$psc++;
							if($pp != $po)
							{
								$po=$pp;
								plog($pp, $t.'...', $pp, __LINE__, __FILE__);
							}
						}
					}
				}
				if($err)
				{
					$perr=true;
					if($display)
						echo('<br /> <b class="s4">&rarr;</b> <b class="red">ERROR</b> <i class="grey">(Transaction ROLLBACK)</i><br />'.$GLOBALS['nl']);
					if($rb)
					{
						db_rollback('rollback');
						$rb=false;
					}
					$sret=true;
					$emsg='SQL I'.$dbc;
				}
				else if($execute)
				{
					if($display)
						echo('<br /> <b class="s4">&rarr;</b> <b class="green">SUCCESS</b> '.$dbc.' sql\'s executed. <i class="grey">(Transaction OK)</i><br />'.$GLOBALS['nl']);
					if($rb)
					{
						db_rollback('ok');
						$rb=false;
					}
					db_slog('SUCCESS: '.$dbc.' sql\'s executed.', __LINE__, __FILE__);
				}
				else
				{
					if($display)
						echo('<br /> <b class="s4">&rarr;</b> <b class="green">SUCCESS</b> '.$dbc.' sql\'s executed. <i class="grey">(Testcase - Transaction OK)</i><br />'.$GLOBALS['nl']);
				}
			}
			else
				db_slog('No changes found.', __LINE__, __FILE__);
		}
	}

	// Get data differences
	if(!$sret && !$perr)
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$t='Patch-Datenänderungen ermitteln';
		else
			$t='Investigating patch data changes';
		plog(45, $t.'...', 70, __LINE__, __FILE__);
		db_slog('*Investigating patch data changes', __LINE__, __FILE__);
		sleep(1);

		$pa=dbGetDataDiff($vd);
		db_slog('Done.', __LINE__, __FILE__);
		if($display)
		{
			echo('<br /><hr sizer="1" />'.$GLOBALS['nl']);
			echo($pa['screencontent']);
		}

		$sec=0;
		if($pa['status'] < 0)
		{
			$sret=true;
			$emsg=$pa['message'];
		}
		else
		{
			foreach($pa['sqlarray'] as $sql)
			{
				if(substr($sql,0,1) != '*')
					$sec++;
			}
		}

		if(!$sret)
		{
			db_slog('*Executing patch data changes', __LINE__, __FILE__);
			// ...Make changes
			if($sec)
			{
//fxDebug($pa['sqlarray'],'dbGetDataDiff: $pa[\'sqlarray\']');
				if($display)
					echo('<br /><hr sizer="1" /><br /><b>SQL\'s</b> <i class="grey">(Transaction START)</i><br /><br />'.$GLOBALS['nl']);

				if($GLOBALS['fxpglobals']['lang'] == 1)
					$t='Patch-Datenänderungen ausführen';
				else
					$t='Executing patch data changes';
				plog(70, $t.'...', 70, __LINE__, __FILE__);
				sleep(1);

				$dbc=0;
				$err=false;
				if(!$rb)
				{
					db_rollback('start');
					$rb=true;
				}
				foreach($pa['sqlarray'] as $sql)
				{
					$pp=min(99, 70+ceil(29*$dbc/$sec));
					if($display)
						echo('<pre style="width:99%;white-space:normal;border:1px solid #cccccc;margin:0;margin-bottom:2px;padding:4px;border-radius:3px;"><b class="darkgrey">'.($dbc+1).' = '.$pp.'%</b> '.fxHtmlEncode($sql).'</pre>'.$GLOBALS['nl']);

					if(substr($sql,0,1) == '*')	// New Table
					{
						if($dbc && $execute)
						{
							if($rb)
								db_rollback('ok');
							db_rollback('start');
							$rb=true;
						}
						plog($pp, $t.'...', $pp, __LINE__, __FILE__);
					}
					else
					{
						$success=dbExecuteQuery($sql);
						if(!$success)
						{
							db_slog('ERROR! '.$sql, __LINE__, __FILE__);
							$err=true;
							break;
						}
						else
						{
							$dbc++;
							$psc++;
							if($pp != $po)
							{
								$po=$pp;
								plog($pp, $t.'...', $pp, __LINE__, __FILE__);
							}
						}
					}
				}
				if($err)
				{
					$perr=true;
					if($display)
						echo('<br /> <b class="s4">&rarr;</b> <b class="red">ERROR</b> <i class="grey">(Transaction ROLLBACK)</i><br />'.$GLOBALS['nl']);
					if($rb)
					{
						db_rollback('rollback');
						$rb=false;
					}
					$sret=true;
					$emsg='SQL D'.$dbc;
				}
				else if($execute)
				{
					if($display)
						echo('<br /> <b class="s4">&rarr;</b> <b class="green">SUCCESS</b> '.$dbc.' sql\'s executed. <i class="grey">(Transaction OK)</i><br />'.$GLOBALS['nl']);
					if($rb)
					{
						db_rollback('ok');
						$rb=false;
					}
					db_slog('SUCCESS: '.$dbc.' sql\'s executed.', __LINE__, __FILE__);
				}
				else
				{
					if($display)
						echo('<br /> <b class="s4">&rarr;</b> <b class="green">SUCCESS</b> '.$dbc.' sql\'s executed. <i class="grey">(Testcase - Transaction OK)</i><br />'.$GLOBALS['nl']);
				}
			}
			else
				db_slog('No changes found.', __LINE__, __FILE__);
		}
	}

	if(!$execute && $rb)
	{
		if($display)
			echo('<br /><hr sizer="1" /><br /> <b class="s4">&rarr;</b> <b class="green">SUCCESS ALL</b> <i class="grey">(Testcase - Transaction All ROLLBACK)</i><br />'.$GLOBALS['nl']);
		db_rollback('rollback');
		$rb=false;
	}

	// Everything ok
	if(!$sret && $execute && !$perr)
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$t='Patch erfolgreich integriert';
		else
			$t='Patch successfully integrated';
		plog(100, $t.'.', 100, __LINE__, __FILE__);
		db_slog('*Patch successfully integrated', __LINE__, __FILE__);
		sleep(2);

		$GLOBALS['swritelog']='';

		$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

		// Write new patch version into DB and set corresponding variables
		$sql="UPDATE wertetab SET tabwert='".$patchversion."', zeitstempel='".$patchdate."' WHERE wertetabid=11 AND mandanten_id=0";
		@db_query($sql, $dbbtl,$dbbtf);
		$GLOBALS['fxpglobals']['dbparam']['version_patch']=$patchversion;
		$GLOBALS['fxpglobals']['dbparam']['date_patch']=$patchdate;

		$sql="UPDATE wertetab SET tabwert='".$GLOBALS['fxpglobals']['dbparam']['version_db'].'-'.$patchversion."', zeitstempel='".$GLOBALS['datetime']."' WHERE wertetabid=12 AND mandanten_id=0";
		@db_query($sql, $dbbtl,$dbbtf);
	}

	$GLOBALS['swritelog']='';

	// After patch adjustments
	if(!$sret && $execute && !$perr)
	{
		// Delete all client menu and mask files
		deleteClientMenuFiles();
		deleteMaskFiles();

		// Execute correction script
		if(isset($GLOBALS['from_dbt_update']))
			$bak=$GLOBALS['from_dbt_update'];
		$GLOBALS['from_dbt_update']=true;
		require('db_correct.inc');
		if(isset($bak))
			$GLOBALS['from_dbt_update']=$bak;

		// Reset vacation calculation
		@reset_urlaubsberechnung();
	}

	// Rename patch file, so that it will not be executed again
	if(!$sret && $execute && !$perr)
	{
		$pnfn=$GLOBALS['datpath'].$vd.'_pat_'.$GLOBALS['datetime'].'_'.$pvs.'-'.$psc.'.txt';
		@chmod($pnfn,0777);
		@unlink($pnfn);
		@rename($pfn,$pnfn);
	}

	// Error?
	if($sret)
	{
		if($GLOBALS['fxpglobals']['lang'] == 1)
			$t='Patch FEHLER';
		else
			$t='Patch ERROR';
		plog(100, $t.': <b class=red>'.$emsg.'</b>', 100, __LINE__, __FILE__);
		sleep(5);
	}

	plog(-1, '', 0, __LINE__, __FILE__);

	// Delete patch active file
	if(strlen($pfa) && file_exists($pfa))
	{
		@chmod($pfa,0777);
		@unlink($pfa);
	}

	return $sret;
}

/**
 * ???FUNCTION???
 *
 * @param integer $mode - Optional parameter (default = 1):    ???PARAMETER???
 * @param string  $msg  - Optional parameter (default = ''):   ???PARAMETER???
 * @param boolean $lit  - Optional parameter (default = true): ???PARAMETER???
 * @param boolean $dots - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbStatus($mode=1, $msg='', $lit=true, $dots=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!isset($GLOBALS['dbstt']))
		$GLOBALS['dbstt']=1;
	else
		$GLOBALS['dbstt']++;

	$ttmsg=false;
	if(strlen($msg) && (substr($msg,0,1) == '^'))
	{
		$ttmsg=true;
		$msg=substr($msg,1);
	}

	$s='';
	if($dots)
		$s .= '&nbsp;<font style="color:#cccccc;">&hellip;</font>&nbsp;';
	$s .= '<b style="border:1px solid #cccccc;border-radius:4px;padding:0;margin:0;color:#';
	if($mode > 0)	// Success
	{
		if(is_bool($lit))
			$litt='OK';
		else
		{
			$litt=$lit;
			$lit=true;
		}
		$s .= '009f6b;';
		if($ttmsg)
			$s .= 'cursor:help;';
		$s .= '"';
		if($ttmsg)
			$s .= ' onmouseover="var id=document.getElementById(\'dbstt'.$GLOBALS['dbstt'].'\'); id.style.display=\'\';"  onmouseout="var id=document.getElementById(\'dbstt'.$GLOBALS['dbstt'].'\'); id.style.display=\'none\';"';
		$s .= '>&nbsp;';
		if($lit)
		{
			$s .= $litt;
			if(strlen($msg))
			{
				if($ttmsg)
					$s .= '&nbsp;&nbsp;&nbsp;<span style="position:absolute;"><img src="'.$GLOBALS['gfxpath'].'tts.png" align="top" style="position:relative;left:-4px;top:4px;"><div id="dbstt'.$GLOBALS['dbstt'].'" style="position:relative;left:8px;top:-18px;width:98%;max-height:512px;border:1px solid black;background:#f1f1f4;color:#000000;font-weight:normal;padding:6px;border-radius:4px;z-index:1024;overflow-x:auto;overflow-y:scroll;display:none;">';
				else
				$s .= ':&nbsp;<font class="normal" style="color:#3db38c;">';
			}
		}
		if(strlen($msg))
		{
			$s .= $msg;
			if($lit)
			{
				if($ttmsg)
					$s .= '</div></span>';
				else
					$s .= '</font>';
			}
		}
	}
	else if($mode < 0)	// Info
	{
		if(is_bool($lit))
			$litt='INFO';
		else
		{
			$litt=$lit;
			$lit=true;
		}
		$s .= '787878;">&nbsp;';
		if($lit)
		{
			$s .= $litt;
			if(strlen($msg))
				$s .= ':&nbsp;<font class="normal" style="color:#969696;">';
		}
		if(strlen($msg))
		{
			$s .= $msg;
			if($lit)
				$s .= '</font>';
		}
	}
	else	// Error
	{
		if(is_bool($lit))
			$litt='ERROR';
		else
		{
			$litt=$lit;
			$lit=true;
		}
		$s .= 'e1001a;">&nbsp;';
		if($lit)
		{
			$s .= $litt;
			if(strlen($msg))
				$s .= ':&nbsp;<font class="normal" style="color:#e43d50;">';
		}
		if(strlen($msg))
		{
			$s .= $msg;
			if($lit)
				$s .= '</font>';
		}
	}
	$s .= '&nbsp;</b>';

	return $s;
}

/**
 * db_table
 *
 * @param string  $tablename - Optional parameter (default = '*'):   ???PARAMETER???
 * @param boolean $del       - Optional parameter (default = false): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_table($tablename='*', $del=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

	$tablename=trim($tablename);
	if(!strlen($tablename) || ($tablename == '*'))
		$tabletype=0;
	else
	{
		$tabletype=1;
		$tablename=strtolower($tablename);
		$cpos=strpos($tablename, '%');
		if($cpos !== false)
			$tabletype=2;
	}

	// Prüfung, ob Tabelle existiert
	if($tabletype == 1)
	{
		$tbex=db_check($tablename, 't');
		if($tbex)
		{
			if($del)
			{
				if($GLOBALS['_dbtype'] == 'sqlsrv')
					$dbo="dbo.";
				else
					$dbo="";

				$sql="DROP TABLE ".$dbo.$tablename.$GLOBALS['fxpglobals']['dbparam']['dbcascade'];
				$success=db_query($sql, $dbbtl,$dbbtf);
				if($success == false)	// Fehler
					return true;
				else
					return false;
			}
			else
				return $tbex;
		}
		else
			return false;
	}

	// Alle Tabellen finden
	$sql="";
	if($GLOBALS['_dbtype'] == 'sqlsrv')
	{
		$sql="SELECT id, name FROM sysobjects WHERE type='U'";
		if($tabletype == 2)
			$sql .= " AND name LIKE '".$tablename."'";
		$sql .= " ORDER BY name";
	}
	else if($GLOBALS['_dbtype'] == 'pgsql')
	{
		$sql="SELECT reltype AS id, relname AS name FROM pg_class WHERE relkind='r'";
		if($tabletype == 2)
			$sql .= " AND LOWER(relname) LIKE '".$tablename."'";
		$sql .= " ORDER BY LOWER(relname)";
	}

	if(strlen($sql))
	{
		$tba=db_values($sql, '*');
		if(is_array($tba))
		{
			$arr=array();

			foreach($tba as $t)
				$arr[strtolower($t['name'])]=$t['id'];

			return $arr;
		}
	}

	return false;
}

/**
 * db_index_get
 *
 * @param string  $indexname    - Optional parameter (default = '*'):  ???PARAMETER???
 * @param string  $tablename    - Optional parameter (default = '*'):  ???PARAMETER???
 * @param boolean $with_index   - Optional parameter (default = true): ???PARAMETER???
 * @param boolean $with_primary - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_index_get($indexname='*', $tablename='*', $with_index=true, $with_primary=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

	// Protokoll
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
	{
		$message='Funktion db_index_get(): '.$GLOBALS['_dbtype'].' ';
		db_protokoll('write', $message);
	}

	$indexname=trim($indexname);
	if(!strlen($indexname) || ($indexname == '*'))
		$indextype=0;
	else
		$indextype=1;

	$tablename=trim($tablename);
	if(!strlen($tablename) || ($tablename == '*'))
		$tabletype=0;
	else
		$tabletype=1;

	// Alle Indexe finden
	if($GLOBALS['_dbtype'] == 'sqlsrv')
	{
		$sql  = "SELECT";
		$sql .= " si.name AS index_name, LOWER(sot.name) AS table_name, si.status,";
		$sql .= " CASE WHEN (si.status & 2048)<>0 THEN 'PK' ELSE 'IDX' END AS index_type,";
		$sql .= " CASE WHEN INDEXPROPERTY(si.id, si.name, 'IsUnique')<>0 THEN 'UNIQUE' ELSE 'NORMAL' END AS unique_index,";
		$sql .= " CASE WHEN INDEXPROPERTY(si.id, si.name, 'IsClustered')<>0 THEN 'CLUSTERED' ELSE 'NONCLUSTERED' END AS clustered_index,";
		$sql .= " sik.keyno AS column_position, LOWER(sc.name) AS column_name ";
		$sql .= "FROM";
		$sql .= " sysindexkeys sik, sysindexes si, syscolumns sc, sysobjects sot ";
		$sql .= "WHERE";
		$sql .= " sot.id=si.id AND sot.id=sc.id AND sot.xtype='U'";
		$sql .= " AND si.id=sik.id AND si.indid=sik.indid AND si.indid BETWEEN 1 AND 254";
		$sql .= " AND INDEXPROPERTY(si.id, si.name, 'IsStatistics')=0 AND INDEXPROPERTY(si.id, si.name, 'IsAutoStatistics')=0 AND INDEXPROPERTY(si.id, si.name, 'IsHypothetical')=0";
		$sql .= " AND sc.id=sik.id AND sc.colid=sik.colid ";
		if($indextype)
			$sql .= " AND LOWER(si.name) LIKE '".strtolower($indexname)."'";
		if($tabletype)
			$sql .= " AND LOWER(sot.name) LIKE '".strtolower($tablename)."'";
		if(!$with_index)
			$sql .= " AND (si.status & 2048)<>0";
		if(!$with_primary)
			$sql .= " AND (si.status & 2048)=0";
		$sql .= "ORDER BY";
		$sql .= " si.name, sik.keyno";
		$idx=db_values($sql, '*');
//fxDebug($idx, $sql);
		if(is_array($idx))
		{
			$arr=array();
			foreach($idx as $i)
			{
				$iindexname=$i['index_name'];
				if(!is_array($arr[$iindexname]))
				{
					$iclustered=false;
					if($i['clustered_index'] == 'CLUSTERED')
						$iclustered=true;
					$iunique=false;
					if($i['unique_index'] == 'UNIQUE')
						$iunique=true;
					$arr[$iindexname]=array('table'=>$i['table_name'], 'type'=>$i['index_type'], 'clustered'=>$iclustered, 'unique'=>$iunique, 'columns'=>array($i['column_name']=>$i['column_position']));
				}
				else
					$arr[$iindexname]['columns'][$i['column_name']]=$i['column_position'];
			}
//fxDebug($arr, '$arr');
			return $arr;
		}
		else
			return false;
	}
	else if($GLOBALS['_dbtype'] == 'pgsql')
	{
		$sql  = "SELECT pgi.indexrelid, pgc.relname AS index_name, pgi.indrelid, LOWER(pgct.relname) AS table_name, pgi.indkey, pgi.indisclustered, pgi.indisunique, pgi.indisprimary ";
		$sql .= "FROM";
		$sql .= " pg_index pgi, pg_class pgc, pg_class pgct ";
		$sql .= "WHERE";
		$sql .= " pgc.oid=pgi.indexrelid AND pgct.oid=pgi.indrelid ";
		if($indextype)
			$sql .= "AND LOWER(pgc.relname) LIKE '".strtolower($indexname)."' ";
		if($tabletype)
			$sql .= "AND LOWER(pgct.relname) LIKE '".strtolower($tablename)."' ";
		if(!$with_index)
			$sql .= "AND pgi.indisprimary='t' ";
		if(!$with_primary)
			$sql .= "AND pgi.indisprimary='f' ";
		$sql .= "ORDER BY";
		$sql .= " LOWER(pgc.relname)";
		$idx=db_values($sql, '*');
//fxDebug($idx, $sql);
		if(is_array($idx))
		{
			$arr=array();
			foreach($idx as $i)
			{
				$iindexname=$i['index_name'];

				$itype='IDX';
				if($i['indisprimary'] == 't')
					$itype='PK';

				$iclustered=false;
				if($i['indisclustered'] == 't')
					$iclustered=true;

				$iunique=false;
				if($i['indisunique'] == 't')
					$iunique=true;

				// Alle Spalten des Index ermitteln
				$column_csv=trim(str_replace(' ', ',', $i['indkey']));
				if(strlen($column_csv))
				{
					$colsql  = "SELECT attnum, LOWER(attname) AS column_name FROM pg_attribute";
					$colsql .= " WHERE attrelid=".$i['indrelid']." AND attnum IN (".$column_csv.")";
					$colsql .= " ORDER BY attnum";
					$cols=db_values($colsql, '*');
					if(is_array($cols))
					{
						$arr[$iindexname]=array('table'=>$i['table_name'], 'type'=>$itype, 'clustered'=>$iclustered, 'unique'=>$iunique, 'columns'=>array());

						foreach($cols as $c)
							$arr[$iindexname]['columns'][$c['column_name']]=(int)$c['attnum'];
					}
				}
			}
//fxDebug($arr, '$arr');
			return $arr;
		}
		else
			return false;
	}

	return false;
}

/**
 * db_index_set
 *
 * @param various $indexarray    - Mandatory parameter:                ???PARAMETER???
 * @param string  $newtablespace - Optional parameter (default = ''):  ???PARAMETER???
 * @param integer $return_sql    - Optional parameter (default = 1):   ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_index_set($indexarray, $newtablespace='', $return_sql=1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

	// Protokoll
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
	{
		$message='Funktion db_index_set(): '.$GLOBALS['_dbtype'].' ';
		db_protokoll('write', $message);
	}

	if(!is_array($indexarray) || !sizeof($indexarray))
		return true;

	$indexsql='';
	foreach($indexarray as $iname => $i)
	{
		$iend='';
		$tabnamexp=explode('_', strtoupper($i['table']));
		foreach($tabnamexp as $subname)
			$iend .= substr($subname, 0, 3);

		$indexname=$i['type'].'_FXP_'.substr(str_replace('.', '', microtime_double()), 0, 18).'_'.substr($iend, 0, 3);

		// Prüfen, ob Index schon existiert
		$ind_ex=0;
		$ind_oa=false;
		if($i['type'] == 'PK')
			$with_index=false;
		else
			$with_index=true;
		$ind_old=db_index_get('*', $i['table'], $with_index, true);
//fxDebug($ind_old, $i['table']);
		if(is_array($ind_old) && sizeof($ind_old))
		{
			foreach($ind_old as $ioname => $io)
			{
				$columns_match=0;
				if(sizeof($i['columns']) == sizeof($io['columns']))
				{
					$columns_match=1;
					foreach($i['columns'] as $columnname => $columnposition)
					{
						if(!isset($io['columns'][$columnname]))
						{
							$columns_match=0;
							break;
						}
					}
				}
				if($columns_match)
				{
					if(strlen($newtablespace) && strlen($io['tablespace']) && ($newtablespace != $io['tablespace']))
						$ind_ex=1;
					else
						$ind_ex=2;
					$ind_oa=array($ioname => $io);
					break;
				}
			}
			if(!$ind_ex && ($i['type'] == 'PK'))
			{
				$ind_ex=1;
				$ind_oa=$ind_old;
			}
		}
//echo('<b>'.$ioname.'</b>: columns_match='.$columns_match.', ind_ex='.$ind_ex.'<br />'.$GLOBALS['nl']);

		// Vorhandenen Index löschen
		if(is_array($ind_oa) && (($ind_ex == 1) || (($ind_ex == 2) && ($return_sql == 2))))
			$indexsql .= db_index_del($ind_oa);

		// Neuen Index erstellen
		if(($ind_ex < 2) || ($return_sql == 2))
		{
			if($GLOBALS['_dbtype'] == 'sqlsrv')
			{
				$indexcols="";
				if(sizeof($i['columns']))
				{
					foreach($i['columns'] as $columnname => $columnposition)
					{
						if(strlen($indexcols))
							$indexcols .= ", ";
						$indexcols .= $columnname;
					}
				}

				// Primärschlüssel
				if($i['type'] == 'PK')
				{
					$indexsql .= "ALTER TABLE dbo.".$i['table']." WITH NOCHECK ADD CONSTRAINT";
					$indexsql .= " ".$indexname." PRIMARY KEY";
					if($i['clustered'])
						$indexsql .= " CLUSTERED";
					else
						$indexsql .= " NONCLUSTERED";
				}
				else
				{
					$indexsql .= "CREATE";
					if($i['unique'])
						$indexsql .= " UNIQUE";
					if($i['clustered'])
						$indexsql .= " CLUSTERED";
					else
						$indexsql .= " NONCLUSTERED";
					$indexsql .= " INDEX ".$indexname." ON dbo.".$i['table'];
				}
				if(strlen($indexcols))
					$indexsql .= " (".$indexcols.")";
				$indexsql .= " ON [PRIMARY]";
				$indexsql .= ";".$GLOBALS['nl'];
			}
			else if($GLOBALS['_dbtype'] == 'pgsql')
			{
				$indexcols="";
				if(sizeof($i['columns']))
				{
					foreach($i['columns'] as $columnname => $columnposition)
					{
						if(strlen($indexcols))
							$indexcols .= ", ";
						$indexcols .= $columnname;
					}
				}

				// Primärschlüssel
				if($i['type'] == 'PK')
				{
					$indexsql .= "ALTER TABLE ".$i['table']." ADD CONSTRAINT";
					$indexsql .= " ".$indexname." PRIMARY KEY";
				}
				else
				{
					$indexsql .= "CREATE";
					if($i['unique'])
						$indexsql .= " UNIQUE";
					$indexsql .= " INDEX ".$indexname." ON ".$i['table'];
				}
				if(strlen($indexcols))
					$indexsql .= " (".$indexcols.")";
				$indexsql .= ";".$GLOBALS['nl'];
			}
		}
	}

	if(strlen($indexsql))
	{
		if($return_sql)
			return $indexsql;
		else
		{
			$index_sql_arr=explode(";".$GLOBALS['nl'], $indexsql);
			foreach($index_sql_arr as $sql)
			{
				$sql=trim($sql);
				if(strlen($sql))
				{
//echo('<b>Execute SQL</b><br />&quot;'.$sql.'&quot;<br />'.$GLOBALS['nl']);
					$success=db_query($sql, $dbbtl,$dbbtf);
					if(!$success)
						return true;
				}
			}
		}
	}

	return false;
}

/**
 * db_index_del
 *
 * @param various $indexarray - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_index_del($indexarray)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

	// Protokoll
	if($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] > 1)
	{
		$message='Funktion db_index_del(): '.$GLOBALS['_dbtype'].' ';
		db_protokoll('write', $message);
	}

	if(!is_array($indexarray) || !sizeof($indexarray))
		return '';

	$indexsql='';
	foreach($indexarray as $indexname => $i)
	{
		if($GLOBALS['_dbtype'] == 'sqlsrv')
		{
			// Primärschlüssel
			if($i['type'] == 'PK')
				$indexsql .= "ALTER TABLE dbo.".$i['table']." DROP CONSTRAINT ".$indexname;
			else
				$indexsql .= "DROP INDEX ".$indexname." ON dbo.".$i['table'];
			$indexsql .= ";".$GLOBALS['nl'];
		}
		else if($GLOBALS['_dbtype'] == 'pgsql')
		{
			// Primärschlüssel
			if($i['type'] == 'PK')
				$indexsql .= "ALTER TABLE ".$i['table']." DROP CONSTRAINT";
			else
				$indexsql .= "DROP INDEX";
			$indexsql .= " ".$indexname.$GLOBALS['fxpglobals']['dbparam']['dbcascade'].";".$GLOBALS['nl'];
		}
	}

	return $indexsql;
}

/**
 * Check the database if a table, view, procedure or column exist
 *
 * @param string $cname - Mandatory parameter:                 Name of the element to be checked
 * @param string $ctype - Optional parameter (default = 't'):  Type of check: 't'=Table, 'v'=View, 'p'=Procedure (only for SQLServer) + 'c'=Column
 * @param string $tname - Optional parameter (default = ''):   Optional table name if a column should be checked
 *
 * @return Check result, or false
 */
function db_check($cname, $ctype='t', $tname='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cname=strtolower(trim($cname));
	$ctype=strtolower(substr($ctype,0,1));

	$csql="";
	switch($ctype)
	{
		case 't':	// TABLE
			if($GLOBALS['_dbtype'] == 'sqlsrv')
				$csql="SELECT id FROM sysobjects WHERE name LIKE '".$cname."' AND type='U'";
			else if($GLOBALS['_dbtype'] == 'pgsql')
				$csql="SELECT reltype AS id FROM pg_class WHERE relkind='r' AND LOWER(relname) LIKE '".$cname."'";
		break;

		case 'v':	// VIEW
			if($GLOBALS['_dbtype'] == 'sqlsrv')
				$csql="SELECT id FROM sysobjects WHERE name LIKE '".$cname."' AND type='V'";
			else if($GLOBALS['_dbtype'] == 'pgsql')
				$csql="SELECT reltype AS id FROM pg_class WHERE relkind='v' AND LOWER(relname) LIKE '".$cname."'";
		break;

		case 'p':	// PROCEDURE
			if($GLOBALS['_dbtype'] == 'sqlsrv')
				$csql="SELECT id FROM sysobjects WHERE name LIKE '".$cname."' AND type='P'";
		break;

		case 's':	// SEQUENCE
			if($GLOBALS['_dbtype'] == 'sqlsrv')
				$sql="SELECT id FROM sysobjects WHERE xtype='P' AND category=0 AND LOWER(name) LIKE '".$cname."'";
			else if($GLOBALS['_dbtype'] == 'pgsql')
				$sql="SELECT reltype FROM pg_class WHERE relkind='S' AND LOWER(relname) LIKE '".$cname."'";
		break;

		case 'c':	// COLUMN
			$tname=strtolower(trim($tname));
			if($GLOBALS['_dbtype'] == 'sqlsrv')
			{
				$tid=0;
				if(strlen($tname))
				{
					$tsql="SELECT id FROM sysobjects WHERE name LIKE '".$tname."' AND type='U'";
					$tid=(int)db_value($tsql);
				}
				$csql="SELECT id FROM syscolumns WHERE name LIKE '".$cname."'";
				if($tid)
					$csql .= " AND id=".$tid;
			}
			else if($GLOBALS['_dbtype'] == 'pgsql')
			{
				$csql="SELECT atttypid AS id FROM pg_attribute WHERE LOWER(attname) LIKE '".$cname."' AND NOT attisdropped AND attnum>0";
				if(strlen($tname))
				{
					$tsql="SELECT reltype AS id FROM pg_class WHERE relkind='r' AND LOWER(relname) LIKE '".$tname."'";
					$tid=(int)db_value($tsql);
					if($tid)
						$csql .= " AND attrelid='".$tname."'::regclass";
					else
						$csql='';
				}
			}
		break;
	}

	// Return sql result, if we have a sql, else return false
	if(strlen($csql))
	{
		$id=(int)db_value($csql);
		return $id;
	}
	return false;
}

/**
 * db_protokoll
 * Datenbankzugriffe werden mitprotokolliert
 * -----------------------------------------------------------------------------
 * Aufruf:
 * °°°°°°°
 * db_protokoll('create')		-> neues Protokoll wird erstellt und an
 * bestehendes angehängt
 * db_protokoll('write', $txt)	-> $txt wird an bestehendes Protokoll angehängt
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $mode   - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $action - Optional parameter (default = false):  ???PARAMETER???
 */
function db_protokoll($mode, $action=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Falls Protokoll ausgeschaltet, dann ciao
	if(!$GLOBALS['forceprotocol'] && (!isset($GLOBALS['fxpglobals']['dbparam']['dbprotocol']) || ($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] < 1) || ($GLOBALS['in_kopf_v'] && ($GLOBALS['fxpglobals']['dbparam']['dbprotocol'] != 7573)) || $GLOBALS['db_noerror']))
		return;

	$dbvalues=array('dbserver', 'dbname', 'dbtype', 'username', 'password', 'dbprotocol', 'timeprotocol');

	// Lokale Zeit ermitteln
	list($usec, $sec)=explode(' ',microtime()); 
	$dbdatum=fx_strftime("%Y%m%d",$sec);
	$dbzeit=fx_strftime("%H:%M:%S",$sec).substr($usec,1,9);

	// Filedirectory prüfen und gegebenenfalls erzeugen
	$filedir=substr($GLOBALS['logpath'],0,-1);
	$writedir=$filedir.'/';
	if(!is_dir($filedir))
	{
		if(!@mkdir($filedir, 0777))
			return;
	}

	// Filename ermitteln
	$nb=strtolower($GLOBALS['fxpglobals']['uname']);
	if($nb == '') // Falls kein Benutzername vorhanden, z.B. noch bevor Benutzer angemeldet ist
	{
		$countname='';
		$nb='p';
	}
	else
	{
		$nb=$GLOBALS['fxpglobals']['client'].'_'.$nb;
		$countname=$writedir.'~db'.$dbdatum.'_'.$nb.'_i.log';
	}
	$filename=$writedir.'~db'.$dbdatum.'_'.$nb.'.log';

	$mode=strtolower($mode);
	switch($mode)
	{
		// Neues DB-Protokoll anlegen
		case 'create':
		 	if(($GLOBALS['fxpglobals']['dbparam']['dbcount'] == '#') || ($GLOBALS['fxpglobals']['dbparam']['dbcount'] == '*'))
			{
				if($countname != '')
				{
					db_prot_index($countname, 'increase');
					$GLOBALS['fxpglobals']['dbparam']['dbcount']='0';
				}
			}
			if($countname != '')
				$GLOBALS['fxpglobals']['dbparam']['dbcount']=(string)((int)$GLOBALS['fxpglobals']['dbparam']['dbcount'] + 1);

			// DB-Protokoll erstellen
			$dbcount=db_prot_index($countname);
			$entry  = '_________________________________________________________________________________________________________'.$GLOBALS['nl_b'];
			$entry .= '> Aufzeichnungsstart um '.$dbzeit.$GLOBALS['nl_b'];
			if($GLOBALS['fxpglobals']['tr'] > 0)
				$prgname='Transaktion: '.$GLOBALS['fxpglobals']['tr'];
			else
				$prgname=$GLOBALS['PHP_SELF'];
			$entry .= '> Aufgerufenes Programm: "'.$prgname.'"'.$GLOBALS['nl_b'];
			$str='> Ini-Datei: ';
			for($i=0; $i<sizeof($dbvalues); $i++)
			{
				if((strtoupper($dbvalues[$i]) != 'PASSWORD') && (strtoupper($dbvalues[$i]) != 'USERNAME'))
					$str .= strtoupper($dbvalues[$i]) . ': "' . $GLOBALS['fxpglobals']['dbparam'][$dbvalues[$i]] . '" ';
			}
			$entry .= trim($str).$GLOBALS['nl_b'];
			if(!$dbcount)
				$dbcount=0;
			$entry .= '> ['.$dbcount.']'.$GLOBALS['nl_b'];
			$entry .= '¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯' .$GLOBALS['nl_b'];

			$clr=false;
			if($action == true)
				$clr=true;
			fxSave($filename, $entry, $clr);
		break;

		// Eintrag in DB-Protokoll schreiben
		case 'start':
		case 'add':
		case 'write';
		case 'commit':
			if($action == false) // es gibt nix zu schreiben
				return;
			if(!file_exists($filename)) // wenn File noch nicht existiert
			{
				db_protokoll('create', true); // und neues File erstellen
				db_prot_index($countname, 'delete');
			}
			$dbcount=db_prot_index($countname);

			$entry='';
			if(($mode == 'start') || ($mode == 'write'))
			{
				if(!$dbcount)
					$dbcount=0;
				$entry .= '['.$dbcount.']';
				if(strlen($GLOBALS['fxpglobals']['dbparam']['dbcount']))
				{
					$dbcount2=$GLOBALS['fxpglobals']['dbparam']['dbcount'];
					if($dbcount2 == '#')
						$dbcount2='?';
				}
				else
					$dbcount2='*';
				$entry .= '['.$dbcount2.'] '.$dbzeit.' -> ';
			};
			$entry .= $action;
			if(($mode == 'commit') || ($mode == 'write'))
				$entry .= $GLOBALS['nl_b'];

			fxSave($filename, $entry, false);
		break;
	}
}

/**
 * db_prot_index
 * Setztliest den aktuellen Index-Zähler für das DB-Protokoll inaus einem File
 * -> Index-File neu erstellen wird autom. ausgeführt wenn File nicht existiert.
 * -----------------------------------------------------------------------------
 * Aufruf:
 * °°°°°°°
 * db_prot_index($filename, 'delete')	->	Index-File $filename löschen
 * db_prot_index($filename)				->	holt den gespeicherten Wert aus dem
 * Index-File $filename
 * db_prot_index($filename, 'increase')	->	Inkrementiert den gespeicherten
 * Wert im Index-File $filename
 * -----------------------------------------------------------------------------
 * Autor: Walter_T
 *
 * @param various $filename - Mandatory parameter:                   ???PARAMETER???
 * @param string  $mode     - Optional parameter (default = 'get'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_prot_index($filename, $mode='get')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($filename == '')
		return;

	if($mode == 'delete') // File löschen
	{
		fxDelete($filename);
		return;
	}

	if(!file_exists($filename)) // File neu erstellen
	{
		$fe=fxSave($filename, '1');
		if($fe)	// Save error?
			return '?';
		return  1;
	}

	if($mode == 'get')
	{
		$fp=fopen($filename, 'r');
		if($fp !== false) // File konnte geöffnet werden
		{
			$rw=(int) fread($fp, 255);
			fclose($fp);
			return $rw;
		}
		return '?';
	}
	else
	{
		$fp=fopen($filename, 'r');
		if($fp !== false) // File konnte geöffnet werden
		{
			$rw=(int) fread($fp, 255);
			fclose($fp);
		}
		else
			return '?';

		$fe=fxSave($filename, $rw+1);
		if($fe)	// Save error?
			return '?';
		return $rw+1;
	}
}

/**
 * db_check_delete
 * Prüft, ob der Datensatz gelöscht werden darf und ruft dann die Funktion
 * db_x() auf.
 * Parameter:
 * °°°°°°°°°°
 * $tab:			Tabelle, aus der Gelöscht wird
 * $feld:			Feldname, durch den die Tabellen verknüpft sind
 * $id:				Wert, der in anderen Tabellen nicht mehr vorkommen darf
 * $query:			Löschanweisung (db_x)
 * -----------------------------------------------------------------------------
 * Autor: Romina
 *
 * @param various $tab       - Mandatory parameter:                   ???PARAMETER???
 * @param various $feld      - Mandatory parameter:                   ???PARAMETER???
 * @param various $id        - Mandatory parameter:                   ???PARAMETER???
 * @param various $query     - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $meldung   - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $sqlzusatz - Optional parameter (default = ''):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_check_delete($tab, $feld, $id, $query, $meldung=false, $sqlzusatz='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql="SELECT id FROM felder WHERE ".db_suche('feldname', $feld)." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	$feldid=db_value($sql);

	$sqlzusatz=trim($sqlzusatz);
	if(strlen($sqlzusatz))
		$sqlzusatz=" AND ".$sqlzusatz;

	$tab_strg="";
	if(is_array($tab))
	{
		foreach($tab as $tbname)
			$tab_strg .= " AND NOT (".db_suche('t.tabname', $tbname)." AND f.id=".$feldid.")";
	}
	else
		$tab_strg .= " AND NOT (".db_suche('t.tabname', $tab).")";

	// Alle Felder mit zugehöriger Tabelle holen und ins Array schreiben
	$tabs=array();
	$cnt=0;

	$sql  = "SELECT f.id, t.tabname, f.feldname, f.alias_zu";
	$sql .= " FROM tabellen t, tab_felder tf, felder f";
	$sql .= " WHERE f.id=tf.idinfelder AND t.id_tabellen=tf.idintabellen AND t.tabelleart IN (1,2,4) AND (".substr($tab_strg, 5).") AND (f.id=".$feldid." OR f.alias_zu=".$feldid.")";
	$wertearr=db_values($sql, '*');
//fxDebug($wertearr,'$wertearr', 0);
	if(fxIsArray($wertearr))
	{
		foreach($wertearr as $wa)
		{
			$tabs[$cnt]=array('tab'=>strtolower($wa['tabname']), 'feld'=>$wa['feldname'], 'sql'=>"SELECT ".$wa['feldname']." FROM ".$wa['tabname']." WHERE ".$wa['feldname']."=".$id.$sqlzusatz, 'wert'=>false);
			// Bei Tabellen mit Mandanten-ID diese zusätzlich abfragen
			if(isset($GLOBALS['fxpglobals']['dbparam']['ctables'][$tabs[$cnt]['tab']]))
				$tabs[$cnt]['sql'] .= " AND mandanten_id=".$GLOBALS['fxpglobals']['client'];

			$cnt++;
		}
	}

	// Folgende Tabellen ignorieren
	$ignore_tables=array(
		'reportspalten'=>true,
		's_einstellungswerte'=>true,
		'teinstellung'=>true
	);

	// Paket 4: "Reisekosten" nicht verfügbar, dann auch diese Tabellen ignorieren
	if(!tr_program(150))
	{
		$ignore_tables['reisekosten']=true;
		$ignore_tables['rkpauschalen']=true;
		$ignore_tables['rktagesangaben']=true;
	}

	// Paket 5: "Material" nicht verfügbar, dann auch diese Tabellen ignorieren
	if(!tr_program(78))
	{
		$ignore_tables['materialbestand']=true;
		$ignore_tables['materialverbrauch']=true;
		$ignore_tables['sachmittel']=true;
	}

	// Paket 6: "DMS" nicht verfügbar, dann auch diese Tabellen ignorieren
	if(!tr_program(110))
	{
		$ignore_tables['d_dok_elt_art']=true;
		$ignore_tables['d_dok_transfer_log']=true;
		$ignore_tables['d_dok_verlauf']=true;
		$ignore_tables['d_dokumente']=true;
	}

	// Paket 7: "Ausgangsrechnungen" nicht verfügbar, dann auch diese Tabellen ignorieren
	if(!tr_program(104))
	{
		$ignore_tables['rechnung']=true;
		$ignore_tables['rechnungsposition']=true;
	}

	// Paket 7: "Eingangsrechnungen" nicht verfügbar, dann auch diese Tabellen ignorieren
	if(!tr_program(117))
	{
		$ignore_tables['rechnungseingang']=true;
	}

	// Überprüfen, ob es Datensätze in diesen Tabellen gibt
	$notab='';
	if(sizeof($tabs))
	{
		foreach($tabs as $cnt => $tb)
		{
			// Historen-Tabellen nicht berücksichtigen
			if(substr($tb['tab'], -8) == '_history')
				continue;
			// Bestimmte Tabellen ebenfalls nicht berücksichtigen
			if(isset($ignore_tables[$tb['tab']]))
				continue;
			// Prüfen, ob die Tabelle auch existiert
			$tex=db_check($tb['tab'], 't');
			if(!$tex)
				continue;

			$wert=@db_value($tb['sql']);
			if($wert)
			{
				$tabs[$cnt]['wert']=$wert;
				if(strlen($notab))
					$notab .= ', ';
				$notab .= '['.$tb['tab'].']';
			}
			else
				db_protokoll('write', 'Funktion db_check_delete(): Tabelle oder Spalte nicht vorhanden -> sql="'.$tb['sql'].'"');
		}
	}

	// Fehlermeldung ausgeben oder SQL ausführen
	if(strlen($notab))
	{
		error_msg(1078, false, $notab);	// 1078: Der Datensatz darf nicht gelöscht werden, da er in $var1 verwendet wird.
		return true;
	}
	else
	{
		$err=db_x($query, $meldung);
		return $err;
	}
}

/**
 * db_suche
 * Autor: Walter_T
 *
 * @param various $db_feldname - Mandatory parameter: ???PARAMETER???
 * @param various $db_feldwert - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_suche($db_feldname, $db_feldwert)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Falls keine Werte übergeben wurde, dann Tschüß
	if(!strlen($db_feldname) || !strlen($db_feldwert))
		return '';

	switch($GLOBALS['_dbtype'])
	{
		case 'pgsql':
			return $db_feldname."~*'".$db_feldwert."'";

		default:
			return $db_feldname."='".$db_feldwert."'";
	}
}

/**
 * db_like
 * Autor: Walter_T
 *
 * @param various $db_feldname - Mandatory parameter:                   ???PARAMETER???
 * @param various $db_feldwert - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $np          - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_like($db_feldname, $db_feldwert, $np=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Falls keine Werte übergeben wurde, dann Tschüß
	if(!strlen($db_feldname) || !strlen($db_feldwert))
		return '';

	$not="";
	if($np)
		$not="NOT ";

	if(substr($db_feldwert,0,1) == "'")
		$db_feldwert=substr($db_feldwert,1);
	if(substr($db_feldwert,-1) == "'")
		$db_feldwert=substr($db_feldwert,0,-1);

	// Spezialzeichen ersetzen
	$sra=array('['=>'_', ']'=>'_');

	if(($GLOBALS['_dbtype'] != 'sqlsrv') && db_is_text($db_feldname))
		return "UPPER(".$db_feldname.") ".$not."LIKE UPPER('".strtr($db_feldwert,$sra)."')";

	return $db_feldname." ".$not."LIKE '".strtr($db_feldwert,$sra)."'";
}

/**
 * db_orderby
 * Autor: Walter_T
 *
 * @return ???RETURN???
 */
function db_orderby()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// No values submitted?
	$arg_cnt=func_num_args();
	if($arg_cnt < 1)
		return '';

	$ret_str=' ORDER BY ';
	$arg_list=func_get_args();
	if(is_array($arg_list))
	{
		$cswitch='';
		if($GLOBALS['_dbtype'] == 'pgsql')
			$cswitch='UPPER';

		foreach($arg_list as $feldname)
		{
			$desc='';
			if(substr($feldname, 0, 1) == '-')
			{
				$feldname=substr($feldname, 1);
				$desc=' DESC';
			}
			if(strlen($cswitch) && db_is_text($feldname))
				$ret_str .= $cswitch."(".$feldname.")".$desc.", ";
			else
				$ret_str .= $feldname.$desc.", ";
		}
		$ret_str=substr($ret_str, 0, -2);
	}

	return $ret_str;
}

/**
 * db_pw
 * Autor: Walter_T
 */
function fxf_dbGetCP()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$ca=array('username', 'password', 'smtp_email', 'smtp_account', 'smtp_password', 'curl_proxy_username', 'curl_proxy_password');

	$GLOBALS['fxdb']=array();
//fxDebug($GLOBALS['fxpglobals']['dbparam'],'$GLOBALS[\'fxpglobals\'][\'dbparam\']', 0);
	if(fxIsArray($GLOBALS['fxpglobals']['dbparam']))
	{
		// Get check code id
		$id2=fxf_paramCodeId(2);

		$chg=array();
		foreach($ca as $c)
		{
			if(isset($GLOBALS['fxpglobals']['dbparam'][$c]))
			{
				$cc=trim($GLOBALS['fxpglobals']['dbparam'][$c]);
				$id=fxf_paramCodeId(0,$cc);
//fxDebug($id,'$id: $id2='.$id2.', $c='.fxHtmlEncode($c).', $cc='.fxHtmlEncode($cc), 0);
				$nc=fxf_paramCodec($cc);

				if($id === $id2)	// Already encoded
					$GLOBALS['fxdb'][$c]=$nc;
				else				// Encode value
				{
					$GLOBALS['fxdb'][$c]=$cc;
					$GLOBALS['fxpglobals']['dbparam'][$c]=$cc;
					$chg[$c]=$nc;
				}
			}
		}
//fxDebug($chg,'$chg',0);

		// Save ini file if values have been encoded
		if(sizeof($chg))
		{
			$filename=fxf_fxIniFile();
			if(strlen($filename))
			{
				$fla=fxLoad($filename, 2);
//fxDebug($fla,'$fla',0);
				if(fxIsArray($fla))
				{
					$save=false;
					foreach($fla as $ln => $line)
					{
						if(strlen($line))
						{
							$ep=strpos($line,'=');
							if($ep)
							{
								$set=strtolower(trim(substr($line,0,$ep)));
								if(isset($chg[$set]))
								{
									$fla[$ln]=substr($line,0,$ep).'= '.$chg[$set];
									$save=true;
								}
							}
						}
					}
//fxDebug($fla,'$fla: $save='.$save,0);

					if($save)
					{
						$fs=sizeof($fla)-1;
						while(($fs >= 0) && !strlen($fla[$fs]))
						{
							unset($fla[$fs]);
							$fs--;
						}
						fxSave($filename, $fla, true);
					}
				}
			}
		}
	}
//fxDebug($GLOBALS['fxdb'],'fxdb',0);
}

/**
 * ???FUNCTION???
 *
 * @param various $text  - Mandatory parameter:                     ???PARAMETER???
 * @param integer $dbbtl - Optional parameter (default = 0):        ???PARAMETER???
 * @param string  $dbbtf - Optional parameter (default = ''):       ???PARAMETER???
 * @param string  $mode  - Optional parameter (default = 'write'):  ???PARAMETER???
 */
function db_slog($text, $dbbtl=0, $dbbtf='', $mode='write')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($dbbtf))
	{
		$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];
	}

	// Logdaten in Datei schreiben?
	if(isset($GLOBALS['swritelog']) && strlen($GLOBALS['swritelog']))
	{
		$sr=150;
		$log_filename=$GLOBALS['logpath'].$GLOBALS['swritelog'].'.log';
		$lct='';
		if(substr($GLOBALS['swritelog'],0,1) == '_')
		{
			if(strncasecmp($text,'DECLARE',7) !== 0)
			{
				$onlyselect=false;
				if(substr($GLOBALS['swritelog'],0,2) == '__')
					$onlyselect=true;
				if(!$onlyselect || (strncasecmp($text,'SELECT',6) === 0))
					$lct .= $text.$GLOBALS['nl_b'];
			}
		}
		else
		{
			if(substr($text,0,1) == '*')
				$l=$GLOBALS['nl_b'].str_repeat('_',$sr).$GLOBALS['nl_b'].strtoupper(substr($text,1)).$GLOBALS['nl_b'].str_repeat('¯',$sr).$GLOBALS['nl_b'];
			else if(substr($text, 0, 1) == '#')
				$l=str_repeat('#',$sr).$GLOBALS['nl_b'].strtoupper(substr($text,1)).$GLOBALS['nl_b'].str_repeat('#',$sr).$GLOBALS['nl_b'];
			else
			{
				$l='';
				if(($mode == 'start') || ($mode == 'write'))
					$l .= fxNow()."\t{".$GLOBALS['fxpglobals']['tr']."}:\t".$dbbtf." [".$dbbtl."]\t";
				$l .= $text;
				if(($mode == 'end') || ($mode == 'write'))
					$l .= $GLOBALS['nl_b'];
			}
			$lct .= $l;
		}
		fxSave($log_filename, $lct, false, false);
	}
	// Logdaten auf Bildschirm ausgeben?
	else if(strlen($GLOBALS['displaylog']))
	{
		if(substr($text, 0, 1) == '*')
			echo('<br /><TABLE BORDER="0" CELLPADDING="0" CELLSPACING="0"><TR><TD CLASS="rhdl">&nbsp;'.strtoupper(substr($text, 1)).'&nbsp;</TD></TR></TABLE>'.$GLOBALS['nl']);
		else
			echo(fxNow().': '.$dbbtf.' ['.$dbbtl.'] '.$text.'<br />'.$GLOBALS['nl']);
	}
}

/**
 * db_subpos
 * Findet das erste Vorkommen eines Substrings in einem SQL-Call.
 * -----------------------------------------------------------------------------
 * -> liefert die gefundene Position (ab 0) zurück oder false
 *
 * @param various $sql       - Mandatory parameter:                  ???PARAMETER???
 * @param various $token     - Mandatory parameter:                  ???PARAMETER???
 * @param integer $token_pos - Optional parameter (default = 0):     ???PARAMETER???
 * @param boolean $tolower   - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_subpos($sql, $token, $token_pos=0, $tolower=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($tolower)
	{
		$token=strtolower($token);
		$sql=trim(strtolower($sql));
	}
	$wsr=array("\t"=>' ', "\r\n"=>'  ', "\n"=>' ');
	$sql=strtr($sql, $wsr);

	while(true)
	{
		$token_pos=strpos($sql, $token, $token_pos);
		if($tokenPos !== false)
		{
			$quote_cnt=substr_count(substr($sql, 0, $token_pos), "'");
			if(($quote_cnt % 2) == 0)
				return $token_pos;
			$token_pos += strlen($token);
		}
		else
			return false;
	}
}

/**
 * db_sql2array
 * Wandelt einen SQL-Call in ein Array um in folgendem Format:
 * [mode]		Modus 'sel', 'ins', 'upd' oder 'del'
 * [table]		Name der Tabelle(n)
 * [tablea]	Array der Tabelle(n)
 * [where]		WHERE-Bedingung
 * [orderby]	ORDER BY Bedingung
 * [columns]	Array mit Spaltenname als Schlüssel und den enstpr. Werten
 * -----------------------------------------------------------------------------
 * -> liefert Array zurück oder false im Fehlerfall
 *
 * @param various $sql - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_sql2array($sql)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql=trim($sql);
	if(!strlen($sql))
		return false;

	$sql_low=strtolower($sql);
	$mode=substr($sql_low, 0, 3);

	$arr=array('mode'=>$mode, 'table'=>"", 'tablea'=>array(), 'where'=>"", 'where_pos'=>false, 'orderby'=>"", 'orderby_pos'=>false, 'columns'=>array());

	$GLOBALS['_parse']=false;

	if(($mode == 'ins') || ($mode == 'upd'))
		$GLOBALS['_maxlength']=0;

	switch($mode)
	{
		case 'sel';	// SELECT
			$column_string='';
			$from_string='';
			$where_string='';
			$orderby_string='';

			$from_pos=db_subpos($sql_low, ' from ', 6, false);
			if($from_pos !== false)
			{
				$column_string=trim(substr($sql_low, 6, $from_pos-6));
				$where_pos=db_subpos($sql_low, ' where ', $from_pos+6, false);
				$arr['where_pos']=$where_pos;
				if($where_pos !== false)
				{
					$from_string=trim(substr($sql_low, $from_pos+6, $where_pos-$from_pos-6));
					$orderby_pos=db_subpos($sql_low, ' order by ', $where_pos+7, false);
					$arr['orderby_pos']=$orderby_pos;
					if($orderby_pos !== false)
					{
						$where_string=trim(substr($sql_low, $where_pos+7, $orderby_pos-$where_pos-7));
						$orderby_string=trim(substr($sql_low, $orderby_pos+10));
					}
					else
						$where_string=trim(substr($sql_low, $where_pos+7));
				}
				else
				{
					$orderby_pos=db_subpos($sql_low, ' order by ', $from_pos+6, false);
					$arr['orderby_pos']=$orderby_pos;
					if($orderby_pos !== false)
					{
						$from_string=trim(substr($sql_low, $from_pos+6, $orderby_pos-$from_pos-6));
						$orderby_string=trim(substr($sql_low, $orderby_pos+10));
					}
					else
						$from_string=trim(substr($sql_low, $from_pos+6));
				}
			}
			else
				$column_string=trim(substr($sql_low, 6));

			$arr['table']=$from_string;
			$arr['where']=$where_string;
			$arr['orderby']=$orderby_string;
		break;

		case 'ins';	// INSERT
			$col_paropen_pos=false;
			$into_pos=db_subpos($sql_low, ' into ', 6, false);
			if($into_pos !== false)
			{
				$col_paropen_pos=db_subpos($sql_low, '(', $into_pos+6, false);
				if($col_paropen_pos !== false)
					$arr['table']=trim(substr($sql_low, $into_pos+6, $col_paropen_pos-$into_pos-6));
			}

			$columns_low='';
			$values='';
			if($col_paropen_pos !== false)
			{
				$values_pos=db_subpos($sql_low, ' values ', $col_paropen_pos+1, false);
				if($values_pos !== false)
				{
					$columns_low=trim(substr($sql_low, $col_paropen_pos, $values_pos-$col_paropen_pos));
					if(substr($columns_low, 0, 1) == '(')
						$columns_low=substr($columns_low, 1);
					if(substr($columns_low, -1) == ')')
						$columns_low=substr($columns_low, 0, -1);

					$values=trim(substr($sql, $values_pos+8));
					if(substr($values, 0, 1) == '(')
						$values=substr($values, 1);
					if(substr($values, -1) == ')')
						$values=substr($values, 0, -1);
				}
			}

			if(strlen($columns_low) && strlen($values))
			{
				$emcnt=0;
				$columns_max_len=strlen($columns_low);
				$column_pos=0;
				$values_max_len=strlen($values);
				$value_pos=0;

				while(true)
				{
					$emcnt++;

					$column_comma_pos=db_subpos($columns_low, ',', $column_pos, false);
					if($column_comma_pos === false)
						$column_comma_pos=$columns_max_len;
					$column=trim(substr($columns_low, $column_pos, $column_comma_pos-$column_pos));
					$column_pos=$column_comma_pos+1;

					$value_comma_pos=db_subpos($values, ',', $value_pos, false);
					if($value_comma_pos === false)
						$value_comma_pos=$values_max_len;
					$value=trim(substr($values, $value_pos, $value_comma_pos-$value_pos));
					$value_pos=$value_comma_pos+1;

					$arr['columns'][$column]=$value;

					if(isset($GLOBALS['fxpglobals']['dbparam']['memo'][$column]))
						$GLOBALS['_parse']=true;

					if(($column_pos > $columns_max_len) || ($value_pos > $values_max_len) || ($emcnt > 999))
						break;
				}
			}
		break;

		case 'upd';	// UPDATE
			$where_pos=db_subpos($sql_low, ' where ', 7, false);
			$arr['where_pos']=$where_pos;
			if($where_pos !== false)
			{
				$arr['where']=ltrim(substr($sql, $where_pos+7));
				$sql_low=rtrim(substr($sql_low, 0, $where_pos));
			}
			$set_pos=db_subpos($sql_low, ' set ', 7, false);
			if($set_pos !== false)
			{
				$arr['table']=trim(substr($sql_low, 7, $set_pos-7));

				$emcnt=0;
				$max_len=strlen($sql_low);
				$column_pos=$set_pos+5;

				while(true)
				{
					$emcnt++;

					$equal_pos=db_subpos($sql_low, '=', $column_pos, false);
					if($equal_pos !== false)
					{
						$comma_pos=db_subpos($sql_low, ',', $equal_pos, false);
						if($comma_pos === false)
							$comma_pos=$max_len;

						$column=trim(substr($sql_low, $column_pos, $equal_pos-$column_pos));
						$value=trim(substr($sql, $equal_pos+1, $comma_pos-$equal_pos-1));
						$arr['columns'][$column]=$value;

						if(isset($GLOBALS['fxpglobals']['dbparam']['memo'][$column]))
							$GLOBALS['_parse']=true;

						$column_pos=$comma_pos+1;
						if($column_pos > $max_len)
							break;
					}
					else
						break;

					if($emcnt > 999)
						break;
				}
			}
		break;

		case 'del';	// DELETE
			$from_string='';
			$where_string='';

			$from_pos=db_subpos($sql_low, ' from ', 6, false);
			if($from_pos !== false)
			{
				$where_pos=db_subpos($sql_low, ' where ', $from_pos+6, false);
				$arr['where_pos']=$where_pos;
				if($where_pos !== false)
				{
					$from_string=trim(substr($sql_low, $from_pos+6, $where_pos-$from_pos-6));
					$where_string=trim(substr($sql_low, $where_pos+7));
				}
				else
				{
					$from_string=trim(substr($sql_low, $from_pos+6));
				}
			}

			$arr['table']=$from_string;
			$arr['where']=$where_string;
		break;
	}

	// Tabellen-Array erstellen aus Tabellenstring
	if(strlen($arr['table']))
	{
		// JOIN's ignorieren, da diese wenigen SQL's manuell abgeändert wurden
		$join_pos=db_subpos($arr['table'], ' join ', 0, false);
		if($join_pos === false)
		{
			$tex=explode(',', $arr['table']);
			foreach($tex as $t)
			{
				$t=trim($t);
				$spos=strpos($t, ' ');
				if($spos)
					$arr['tablea'][trim(substr($t, 0, $spos))]=trim(substr($t, $spos+1));
				else
					$arr['tablea'][trim($t)]='';
			}
		}
	}

	// Strings ersetzen
	if(sizeof($arr['columns']))
	{
		foreach($arr['columns'] as $column => $value)
		{
			if(substr($value, 0, 1) == '\'')
				$value=substr($value, 1);
			if(substr($value, -1) == '\'')
				$value=substr($value, 0, -1);

			$arr['columns'][$column]=$value;

			if((($mode == 'ins') || ($mode == 'upd')) && isset($GLOBALS['fxpglobals']['dbparam']['char'][$column]) && ($value != 'NULL') && (mb_strlen($value) > $GLOBALS['fxpglobals']['dbparam']['char'][$column]))
			{
				// 2751: Der eingegebene Text &quot;<i>$var2</i>&quot; für das Feld [<b>$var1</b>] ist zu lang (<i>$var3 Zeichen</i>)! Die Maximallänge beträgt nur <b>$var4</b> Zeichen.
				error_msg(2751, false, $column, $value, strlen($value), $GLOBALS['fxpglobals']['dbparam']['char'][$column]);
				$GLOBALS['_maxlength']++;
			}
		}
	}

//fxDebug($arr, $sql);
	return $arr;
}

/**
 * Create an insert, update or delete sql from an array in the following format:
 * [mode]     Mode 'sel', 'ins', 'upd' or 'del'
 * [table]    Name of the table
 * [where]    WHERE clause
 * [columns]  Array containing all columns as key and their values
 *
 * @return Created sql string or false on error
 */
function db_array2sql($arr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!fxIsArray($arr) || !strlen($arr['mode']) || !strlen($arr['table']))
		return false;

	$mode=strtolower(substr($arr['mode'], 0,3));
	switch($mode)
	{
		case 'ins':	// INSERT
		case 'upd':	// UPDATE
			if($mode == 'ins')
			{
				$sql="INSERT INTO ".trim($arr['table']);
				if(fxIsArray($arr['columns']))
				{
					$columns="";
					$values="";
					foreach($arr['columns'] as $column => $value)
					{
						// Column
						$column=strtolower($column);
						if(strlen($columns))
							$columns .= ", ";
						$columns .= $column;

						// Value
						if(strlen($values))
							$values .= ", ";
						$values .= db_convert_value($column, $value);
					}
					if(strlen($columns))
						$sql .= " (".$columns.") VALUES (".$values.")";
				}
			}
			else
			{
				$sql="UPDATE ".trim($arr['table']);
				if(fxIsArray($arr['columns']))
				{
					$col_cnt=0;
					foreach($arr['columns'] as $column => $value)
					{
						// Column
						$column=strtolower($column);

						if($col_cnt)
							$sql .= ",";
						else
							$sql .= " SET";
						$sql .= " ".trim($column)."=";
						$col_cnt++;

						// Value
						$sql .= db_convert_value($column, $value);
					}
				}
				if(strlen($arr['where']))
					$sql .= " WHERE ".$arr['where'];
			}
		break;

		case 'del':	// DELETE
			$sql="DELETE FROM ".trim($arr['table']);
			if(strlen($arr['where']))
				$sql .= " WHERE ".$arr['where'];
		break;
	}
//echo('<b>SQL:</b> &quot;'.fxHtmlEncode($sql).'&quot;<br />'.$GLOBALS['nl']);

	return $sql;
}

/**
 * db_sqlAddClient
 * Prüft, ob bei Mandanten-Tabellen im SQL die Mandanten-ID mitgegeben wurde in
 * der WHERE-Bedingung - falls nicht, dann wird diese gesetzt.
 * [sql]		Ursprungs-SQL
 * [arr]		Analyse-Array, welches mit db_sql2array() ermittelt wurde
 * -----------------------------------------------------------------------------
 * -> liefert angepassten SQL-Call zurück
 *
 * @param various $sql   - Mandatory parameter:                ???PARAMETER???
 * @param various $arr   - Mandatory parameter:                ???PARAMETER???
 * @param integer $dbbtl - Optional parameter (default = 0):   ???PARAMETER???
 * @param string  $dbbtf - Optional parameter (default = ""):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_sqlAddClient($sql, $arr, $dbbtl=0, $dbbtf="")
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$debug_sql=false;
	if(!isset($GLOBALS['nodebugsql']) && $GLOBALS['dbg'])
		$debug_sql=true;

	// Bei Mandanten-Tabellen prüfen, ob Mandanten-ID mitgegeben wurde in der WHERE-Bedingung - falls nicht, dann setzen
	if(!isset($GLOBALS['dontAddClient']) && fxIsArray($arr) && fxIsArray($arr['tablea']) && $GLOBALS['fxpglobals']['client'])
	{
		$debug_style=' style="position:relative;width:948px;border:1px solid #bbbbcc;border-radius:6px;padding:1px 4px;white-space:normal;"';

		$ct=array();
		foreach($arr['tablea'] as $tn => $ts)
		{
			if(isset($GLOBALS['fxpglobals']['dbparam']['ctables'][$tn]))
				$ct[$tn]=$ts;
		}

		if($debug_sql)
		{
			if(!strlen($dbbtf))
			{
				$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];
			}

			$scol=array('sel'=>'666666', 'ins'=>'00aa11', 'upd'=>'0011aa', 'del'=>'aa0011');
			$sql_old=$sql;
//echo('<hr><pre>'.fxHtmlEncode($sql).'</pre>mode: <b>'.$arr['mode'].'</b> | table: <b>'.$arr['table'].'</b> | where: <b>'.$arr['where'].'</b> ('.$arr['where_pos'].') | orderby: <b>'.$arr['orderby'].'</b> ('.$arr['orderby_pos'].')<br />');
//fxDebug($arr['tablea'], 'table: '.$arr['table']);
		}

		if($arr['mode'] != 'ins')
		{
			if(fxIsArray($ct))
			{
				foreach($ct as $tn => $ts)
				{
					if(strlen($ts))
						$sc=$ts.'.mandanten_id';
					else
						$sc='mandanten_id';

					if($GLOBALS['fxpglobals']['dbparam']['ctables'][$tn] == 2)			// Daten
						$cin="=".$GLOBALS['fxpglobals']['client'];
					else if($GLOBALS['fxpglobals']['dbparam']['ctables'][$tn] == 4)	// Individuell
						$cin=" IN (0,".$GLOBALS['fxpglobals']['client'].")";
					else													// System+Intern
						$cin="=0";

					if($arr['where_pos'])
					{
						$scp=strpos($arr['where'], $sc);
						if($scp === false)
							$sql=substr($sql, 0, $arr['where_pos']+7).$sc.$cin." AND".substr($sql, $arr['where_pos']+6);
					}
					else if($arr['orderby_pos'])
					{
						$sql=substr($sql, 0, $arr['orderby_pos'])." WHERE ".$sc.$cin.substr($sql, $arr['orderby_pos']);
						$arr['where_pos']=$arr['orderby_pos'];
						$arr['orderby_pos'] += (strlen($GLOBALS['fxpglobals']['client']) + strlen($sc) + 15);
					}
					else
					{
						$arr['where_pos']=strlen($sql);
						$sql .= " WHERE ".$sc.$cin;
					}
				}
			}

			if($debug_sql)
			{
				if($arr['mode'] != 'sel')
				{
					if($sql != $sql_old)
					{
						$col=$scol[$arr['mode']];
						echo($dbbtf.' ('.$dbbtl.'):<br /><div'.$debug_style.'><b style="color:#'.$col.';">'.fxHtmlEncode($sql).'</b></div><br />');
					}
					else
					{
						$col='000000';
						echo($dbbtf.' ('.$dbbtl.'):<br /><div'.$debug_style.'><b style="color:#'.$col.';">'.fxHtmlEncode($sql).'</b></div><br />');
					}
				}
				else if(true && ($sql != $sql_old))
					echo($dbbtf.' ('.$dbbtl.'):<br /><div'.$debug_style.'><font style="color:#'.$scol[$arr['mode']].';">'.fxHtmlEncode($sql).'</font></div><br />');
			}
		}
		else if($debug_sql)
			echo($dbbtf.' ('.$dbbtl.'):<br /><div'.$debug_style.'><b style="color:#333333;">'.fxHtmlEncode($sql).'</b></div><br />');
	}

	return $sql;
}

/**
 * ???FUNCTION???
 *
 * @param various $column - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_is_text($column)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$column=trim(strtolower($column));
	$dotpos=strpos($column, '.');
	if($dotpos)
		$column=substr($column, $dotpos+1);
	if(!strlen($column) || $GLOBALS['fxpglobals']['dbparam']['integer'][$column] || $GLOBALS['fxpglobals']['dbparam']['float'][$column])
		return false;
	return true;
}

/**
 * ???FUNCTION???
 *
 * @param various $column - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_is_integer($column)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$column=trim(strtolower($column));
	$dotpos=strpos($column, '.');
	if($dotpos)
		$column=substr($column, $dotpos+1);
	if($GLOBALS['fxpglobals']['dbparam']['integer'][$column])
		return true;
	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $column - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_is_float($column)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$column=trim(strtolower($column));
	$dotpos=strpos($column, '.');
	if($dotpos)
		$column=substr($column, $dotpos+1);
	if($GLOBALS['fxpglobals']['dbparam']['float'][$column])
		return true;
	return false;
}

/**
 * Convert a database entry according to its defined column, e.g. float, integer or string
 *
 * @param string  $column     - Mandatory parameter:                  Lowercase column name
 * @param various $value      - Mandatory parameter:                  Database entry value
 * @param boolean $conv_null  - Optional parameter (default = true):  Convert NULL values (true) to string etc. or not (false)
 *
 * @return Type converted database entry
 */
function db_convert_column($column, $value, $conv_null=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$conv_null && !strlen($value))
		$result=NULL;
	else if(isset($GLOBALS['fxpglobals']['dbparam']['float'][$column]))
		$result=get_double($value);
	else if(isset($GLOBALS['fxpglobals']['dbparam']['integer'][$column]))
		$result=(int)$value;
	else
		$result=(string)$value;

	return $result;
}

/**
 * ???FUNCTION???
 *
 * @param various $column - Mandatory parameter: ???PARAMETER???
 * @param various $value  - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_convert_value($column, $value)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($value) || ($value === 'NULL'))
		return "NULL";
	else if(isset($GLOBALS['fxpglobals']['dbparam']['float'][$column]))
		return get_double($value);
	else if(isset($GLOBALS['fxpglobals']['dbparam']['integer'][$column]))
		return (int)$value;
	else
		return "'".convert_string($value,'todb')."'";
}

/**
 * ???FUNCTION???
 *
 * @param various $value - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_convert_string($value)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($value) || ($value === 'NULL'))
		return "NULL";
	else
		return "'".convert_string($value,'todb')."'";
}

/**
 * ???FUNCTION???
 *
 * @param various $value - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_reconvert_string($value)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($value) || ($value === 'NULL'))
		return "";
	else
	{
		if(substr($value,0,1) == "'")
			$value=substr($value,1);
		if(substr($value,-1) == "'")
			$value=substr($value,0,-1);
		return convert_string($value,'fromdb');
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $value - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_convert_number($value)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($value) || ($value === 'NULL'))
		return "NULL";
	else
		return $value;
}

/**
 * ???FUNCTION???
 *
 * @param various $value - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_reconvert_number($value)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!strlen($value) || ($value === 'NULL'))
		return "";
	else
		return $value;
}

/**
 * Prüft alle Benutzerviews der einzelnen Mandanten und setzt diese gegebenenfalls neu.
 *
 * @param integer $mid - Optional parameter (default = 0): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_userviews($mid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$mid=(int)$mid;
	$ret=false;

	$sql="SELECT repviewid, view_sql FROM repviews WHERE mandanten_id>0";
	if($mid > 0)
		$sql .= " AND mandanten_id=".$mid;
	$views=db_values($sql, '*');
	if(is_array($views))
	{
		$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

		foreach($views as $v)
		{
			$vname='V_FXP_'.(int)$v['repviewid'];
			$vsql=trim($v['view_sql']);

			// Prüfen, ob View bereits existiert...
			$vex=db_check($vname, 'v');
			if($vex)
				continue;

			// ...Nein, dann neu erstellen
			$sql="CREATE VIEW ".$vname." AS ".$vsql;
			$success=db_query($sql, $dbbtl,$dbbtf);
			if(!$success)
			{
				$ret=true;
				break;
			}
		}
	}

	return $ret;
}

/**
 * ???FUNCTION???
 *
 * @param integer $status  - Optional parameter (default = 0):  ???PARAMETER???
 * @param string  $action  - Optional parameter (default = ''): ???PARAMETER???
 * @param string  $message - Optional parameter (default = ''): ???PARAMETER???
 */
function db_errorQueue($status=0, $action='', $message='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$timestamp=fx_date('YmdHis',time());

	if(!fxIsArray($GLOBALS['dbeq']))
		$GLOBALS['dbeq']=array();
	if(!isset($GLOBALS['dbeqid']))
		$GLOBALS['dbeqid']=0;

	if(!fxIsArray($GLOBALS['dbeq'][$GLOBALS['dbeqid']]))
		$GLOBALS['dbeq'][$GLOBALS['dbeqid']]=array('start'=>$timestamp, 'end'=>$timestamp, 'action'=>$action, 'message'=>$message, 'status'=>$status);
	else
	{
		$GLOBALS['dbeq'][$GLOBALS['dbeqid']]['end']=$timestamp;
		if(strlen($action))
			$GLOBALS['dbeq'][$GLOBALS['dbeqid']]['action']=$action;
		if(strlen($message))
		{
			if(strlen($GLOBALS['dbeq'][$GLOBALS['dbeqid']]['message']))
				$GLOBALS['dbeq'][$GLOBALS['dbeqid']]['message'] .= '<br />';
			if($status > 0)
				$GLOBALS['dbeq'][$GLOBALS['dbeqid']]['message'] .= '<font class="green">';
			else if($status < 0)
				$GLOBALS['dbeq'][$GLOBALS['dbeqid']]['message'] .= '<font class="red">';
			$GLOBALS['dbeq'][$GLOBALS['dbeqid']]['message'] .= '&middot;&nbsp;'.$message;
			if($status)
				$GLOBALS['dbeq'][$GLOBALS['dbeqid']]['message'] .= '</font>';
		}
		if($status)
			$GLOBALS['dbeq'][$GLOBALS['dbeqid']]['status']=$status;
	}

	if($status)
		$GLOBALS['dbeqid']++;
}

/**
 * ???FUNCTION???
 *
 * @param various $rtn - Mandatory parameter: ???PARAMETER???
 * @param various $la  - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbCreateIndexSQL($rtn,$la)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql="";
	if(strlen($rtn) && fxIsArray($la))
	{
		$sql="CREATE";
		if(($GLOBALS['_dbtype'] == 'sqlsrv') && $la[5])
			$sql .= " CLUSTERED";
		$sql .= " INDEX ".$la[3]." ON ";
		if($GLOBALS['_dbtype'] == 'sqlsrv')
			$sql .= "dbo.";
		$sql .= $rtn." (".$la[4].")";
		if($GLOBALS['_dbtype'] == 'sqlsrv')
			$sql .= " ON [PRIMARY]";
	}

	return $sql;
}

/**
 * ???FUNCTION???
 *
 * @param various $rtn - Mandatory parameter: ???PARAMETER???
 * @param various $la  - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbDropIndexSQL($rtn,$la)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql="";
	if(strlen($rtn) && fxIsArray($la))
	{
		$sql="DROP INDEX ".$la[3];
		if($GLOBALS['_dbtype'] == 'sqlsrv')
			$sql .= " ON dbo.".$rtn;
		$sql .= $GLOBALS['fxpglobals']['dbparam']['dbcascade'];
	}

	return $sql;
}

/**
 * ???FUNCTION???
 *
 * @param various $sql - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbCreateView($sql)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$success=true;

	$lsql=strtolower($sql);
	if(substr($lsql,0,12) == 'create view ')
	{
		$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];

		$np=12;
		$ap=strpos($lsql,' as ',$np);
		if($ap !== false)
		{
			$vn=trim(substr($sql,$np,$ap-$np));
			$vs=trim(substr($sql,$ap+4));
			if(substr($vs,-1) == ';')
				$vs=trim(substr($vs,0,-1));
			$es=dbCreateViewSQL($vn,$vs);
			if(strlen($es))
			{
				$ds=dbDropViewSQL($vn);
				if(strlen($ds))
					$success=db_query($ds, $dbbtl,$dbbtf);
				if($success)
					$success=db_query($es, $dbbtl,$dbbtf);
			}
		}
	}

	return $success;
}

/**
 * ???FUNCTION???
 *
 * @param various $vn    - Mandatory parameter:                  ???PARAMETER???
 * @param various $vsql  - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $check - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbCreateViewSQL($vn,$vsql,$check=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql="";
	if(strlen($vn) && strlen($vsql))
	{
		$tca=array();
		if($check)
			$tca=dbViewTables($vsql);

		$create=true;
		if(sizeof($tca))
		{
//fxDebug($tca,'$tca');
			foreach($tca as $tn => $dummy)
			{
				if((substr($tn,0,3) == 'vs_') || (substr($tn,0,3) == 'vd_') || (substr($tn,0,3) == 'vn_') || (substr($tn,0,2) == 'v_'))	// Check view
					$ex=db_check($tn,'v');
				else																													// Check table
					$ex=db_check($tn);
				if(!$ex)
				{
					$create=false;
					break;
				}
			}
		}

		if($create)
			$sql="CREATE VIEW ".$vn." AS ".$vsql;
	}

	return $sql;
}

/**
 * ???FUNCTION???
 *
 * @param various $sql - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbViewTables($sql)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$tca=array();
	$lsql=strtolower($sql);
	$fda=explode(' from ',$lsql);
	if(sizeof($fda) > 0)
	{
		$tta=array();
		foreach($fda as $fc => $fs)
		{
			if($fc)
			{
				$fs=trim(strtr($fs,array('('=>' ', ')'=>' ', "\n"=>' ', "\t"=>' ')));
				$wp=strpos($fs,' where ');
				if($wp !== false)
					$fs=substr($fs,0,$wp);
				$te=explode(' join ',$fs);
				foreach($te as $t)
					$tta[]=trim($t);
				$te=explode(',',$fs);
				foreach($te as $t)
					$tta[]=trim($t);
			}
		}
		if(sizeof($tta))
		{
			foreach($tta as $t)
			{
				$sp=strpos($t,' ');
				if($sp !== false)
					$t=substr($t,0,$sp);
				$tca[trim($t)]=true;
			}
		}
	}

	return $tca;
}

/**
 * ???FUNCTION???
 *
 * @param various $sql - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbDropView($sql)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$success=true;

	$lsql=strtolower($sql);
	if(substr($lsql,0,10) == 'drop view ')
	{
		if(substr($sql,-1) == ';')
			$sql=trim(substr($sql,0,-1));
		$vn=trim(substr($sql,10));
		$sp=strpos($vn,' ');
		if($sp !== false)
			$vn=trim(substr($vn,0,$sp));
		$ds=dbDropViewSQL($vn);
		if(strlen($ds))
		{
			$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];
			$success=db_query($ds, $dbbtl,$dbbtf);
		}
	}

	return $success;
}

/**
 * ???FUNCTION???
 *
 * @param various $vn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbDropViewSQL($vn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql="";
	if(strlen($vn) && db_check($vn,'v'))
		$sql="DROP VIEW ".$vn.$GLOBALS['fxpglobals']['dbparam']['dbcascade'];

	return $sql;
}

/**
 * ???FUNCTION???
 *
 * @param various $sql   - Mandatory parameter:                ???PARAMETER???
 * @param integer $dbbtl - Optional parameter (default = 0):   ???PARAMETER???
 * @param string  $dbbtf - Optional parameter (default = ''):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbExecuteQuery($sql, $dbbtl=0, $dbbtf='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lsql=strtolower($sql);
	if(substr(trim($lsql),0,12) == 'create view ')
		$success=dbCreateView($sql);
	else if(substr($lsql,0,10) == 'drop view ')
		$success=dbDropView($sql);
	else
	{
		if(!strlen($dbbtf))
		{
			$dbbta=debug_backtrace(); $dbbtf=$dbbta[0]['file']; $dbbtl=$dbbta[0]['line'];
		}
		$success=db_query($sql, $dbbtl,$dbbtf);
	}

	return $success;
}

/**
 * ???FUNCTION???
 *
 * @param various $pkn  - Mandatory parameter:                   ???PARAMETER???
 * @param various $pkcl - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $ttpa - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbCreatePrimaryKeyDef($pkn, $pkcl, $ttpa=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pks="";
	if(strlen($pkn))
	{
		$pks .= "CONSTRAINT ".$pkn." PRIMARY KEY";
		if($GLOBALS['_dbtype'] == 'sqlsrv')
		{
			if($pkcl)
				$pks .= " CLUSTERED";
			else
				$pks .= " NONCLUSTERED";
		}
		$pks .= $GLOBALS['nl_f']."\t (";
		$pcc=0;
		if(fxIsArray($ttpa))
		{
			foreach($ttpa as $pcn => $pdummy)
			{
				if($pcc)
					$pks .= ",";
				$pks .= $GLOBALS['nl_f']."\t\t ".$pcn;
				if($GLOBALS['_dbtype'] == 'sqlsrv')
					$pks .= " ASC";

				$pcc++;
			}
		}
		$pks .= $GLOBALS['nl_f']."\t )";
		if($GLOBALS['_dbtype'] == 'sqlsrv')
			$pks .= " WITH (PAD_INDEX=OFF, STATISTICS_NORECOMPUTE=OFF, IGNORE_DUP_KEY=OFF, ALLOW_ROW_LOCKS=ON, ALLOW_PAGE_LOCKS=ON) ON [PRIMARY]";
	}

	return $pks;
}

/**
 * ???FUNCTION???
 *
 * @param various $ltn  - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $ttca - Optional parameter (default = false):  ???PARAMETER???
 * @param string  $pks  - Optional parameter (default = ''):     ???PARAMETER???
 * @param boolean $ttpa - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function dbCreateTableSQL($ltn, $ttca=false, $pks='', $ttpa=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Table
	$sql="CREATE TABLE ";
	if($GLOBALS['_dbtype'] == 'sqlsrv')
		$sql .= "dbo.";
	$sql .= $ltn.$GLOBALS['nl_f']." (";
	// Add columns
	$cc=0;
	if(fxIsArray($ttca))
	{
		foreach($ttca as $cn => $cda)
		{
			// Field type
			if(isset($cda['ftype']))
				$ftype=$cda['ftype'];
			else
				$ftype=$cda[3];

			// Not null
			$nnl=0;
			if((isset($cda['not_null']) && $cda['not_null']) || (isset($cda[4]) && $cda[4]) || (fxIsArray($ttpa) && isset($ttpa[$cn])))
				$nnl=1;

			if($cc)
				$sql .= ",";
			$sql .= $GLOBALS['nl_f']."\t ".$cn." ".dbColumnType($ftype);
			if($nnl)
				$sql .= " NOT NULL";
			else if($GLOBALS['_dbtype'] == 'sqlsrv')
				$sql .= " NULL";

			$cc++;
		}
	}

	// Primarykey definition
	if(strlen($pks))
	{
		if($cc)
			$sql .= ",".$GLOBALS['nl_f'];
		$sql .= $GLOBALS['nl_f']."\t ".$pks;
	}

	$sql .= $GLOBALS['nl_f']." )";
	if($GLOBALS['_dbtype'] == 'sqlsrv')
		$sql .= " ON [PRIMARY]";

	return $sql;
}

/**
 * ???FUNCTION???
 *
 * @param various $fa  - Mandatory parameter: ???PARAMETER???
 * @param various $oa  - Mandatory parameter: ???PARAMETER???
 * @param various $pre - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function db_getDiffSQL($fa, $oa, $pre)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql="";
	foreach($fa as $ck => $cf)
	{
		$ct=substr($cf,0,1);
		$cf=substr($cf,1);

		if($ct == 'i')		// Integer
		{
			$dv=(int)$oa[$cf];
			$cv=(int)$GLOBALS['db_'.$pre.'_'.$ck];
		}
		else if($ct == 'f')	// Float
		{
			$dv=(float)$oa[$cf];
			$cv=(float)$GLOBALS['db_'.$pre.'_'.$ck];
		}
		else if($ct == 'c')	// Checkbox
		{
			$dv=0;
			$cv=0;
			if($oa[$cf])
				$dv=1;
			if($GLOBALS['db_'.$pre.'_'.$ck])
				$cv=1;
		}
		else				// Date or Text
		{
			$dv=convert_string($oa[$cf],'todb');
			$cv=db_reconvert_string($GLOBALS['db_'.$pre.'_'.$ck]);
		}

		$cls='green';
		if($dv != $cv)
			$cls='red';
//echo('<b class="'.$cls.'">'.$ck.'</b> ['.$cf.' ('.$ct.')]: $dv='.$dv.', $cv='.$cv.'<br />');

		if($dv != $cv)
			$sql .= ", ".$cf."=".$GLOBALS['db_'.$pre.'_'.$ck];
	}

	return $sql;
}

/**
 * ID_MAX
 *
 * @param various $tab_name  - Mandatory parameter:                  ???PARAMETER???
 * @param various $field_name - Mandatory parameter:                  ???PARAMETER???
 * @param string  $bedingung - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $client_id   - Optional parameter (default = ''):    ???PARAMETER???
 * @param integer $min_id    - Optional parameter (default = 1):     ???PARAMETER???
 * @param boolean $tabupdate - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function id_max($tab_name, $field_name, $bedingung='', $client_id='', $min_id=1, $tabupdate=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Werte zuweisen
	$tnl=strtolower($tab_name);
	$fnl=strtolower($field_name);
	$client_id=trim($client_id);
	if($bedingung)
		$bedingung=" WHERE ".trim($bedingung);
	else if(strlen($client_id))
	{
		$bedingung=" WHERE mandanten_id=".$client_id;
		if(($min_id == 1) && $client_id)
			$min_id=$client_id*10000+100000;
	}
	else if(isset($GLOBALS['fxpglobals']['dbparam']['ctables'][$tnl]))
	{
		$bedingung=" WHERE mandanten_id";
		if(($tnl != 'felder') && ($tnl != 'masken'))
			$bedingung .= "=".$GLOBALS['fxpglobals']['client'];
		else
			$bedingung .= " IS NOT NULL";
	}

	// Spezialfall, wenn Werte nicht mittels Sequence ermittelt werden sollen
	if(!$min_id)
	{
		// Max-ID auf herkömmliche Weise holen
		$sql="SELECT MAX(".$fnl.") AS oldid FROM ".$tnl.$bedingung;
		$max_id=(int)db_value($sql);
		if(!$max_id)
			$max_id=1;
		else
			$max_id++;

		return $max_id;
	}
	if(strlen($client_id))
		$client_id='m'.$client_id;

	// Sequencename generieren
	$seqname=fxf_dbSeqName($tnl, $fnl, $client_id, true);

	// Prüfen, ob Sequence vorhanden...
	$chkname=fxf_dbSeqCheck($seqname);
	if($chkname === false)
		return false;

	// ...ja, dann Wert mittels Sequence erhöhen und prüfen
	if($chkname && $tabupdate)
	{
		$max_id=fxf_dbSeqValue($seqname, true);
		if($max_id !== false)
		{
			$bed=$bedingung;
			if(strlen($bed))
				$bed .= " AND";
			else
				$bed=" WHERE";
			$bed .= " ".$fnl."=".$max_id;

			$sql="SELECT ".$fnl." AS chk FROM ".$tnl.$bed;
			$chk=(int)db_value($sql);
		}
		else
			$chk=true;
		if($chk)
		{
			// Eine solche ID existiert bereits, deshalb Sequenz löschen
			$ierr=fxf_dbSeqDelete('', $tnl, $fnl, $client_id, true);
			if($ierr)
				return false;

			$chkname='';
		}
		else if(!isset($GLOBALS['db_mia'][$seqname]))
			$GLOBALS['db_mia'][$seqname]=$max_id-1;
	}

	// ...nein, dann Sequence erstellen und Startwert zurückliefern
	if(!$chkname || !$tabupdate)
	{
		// Startwert ermitteln
		$sql="SELECT MAX(".$fnl.") AS oldid FROM ".$tnl.$bedingung;
		$max_id=(int)db_value($sql);
		if(!$max_id)
			$max_id=$min_id;
		else
			$max_id=MAX($max_id+1, $min_id);

		if($tabupdate)
		{
			// Sequence erstellen
			$serr=fxf_dbSeqCreate($seqname, abs($max_id));
			if($serr)
				return false;

			$rmax_id=$max_id;
			$max_id=fxf_dbSeqValue($seqname, true);
			if($max_id === false)
			{
				$ierr=fxf_dbSeqDelete('', $tnl, $fnl, $client_id, true);
				if($ierr)
					return false;
				$max_id=$rmax_id;
			}
			else if(!isset($GLOBALS['db_mia'][$seqname]))
				$GLOBALS['db_mia'][$seqname]=$max_id-1;
		}
	}

	return $max_id;
}

/**
 * ID_MIN
 *
 * @param various $tab_name  - Mandatory parameter:                  ???PARAMETER???
 * @param various $field_name - Mandatory parameter:                  ???PARAMETER???
 * @param string  $bedingung - Optional parameter (default = ''):    ???PARAMETER???
 * @param string  $client_id   - Optional parameter (default = ''):    ???PARAMETER???
 * @param various $min_id    - Optional parameter (default = -1):    ???PARAMETER???
 * @param boolean $tabupdate - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function id_min($tab_name, $field_name, $bedingung='', $client_id='', $min_id=-1, $tabupdate=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Werte zuweisen
	$tnl=strtolower($tab_name);
	$fnl=strtolower($field_name);
	$client_id=trim($client_id);
	if($bedingung)
		$bedingung=" WHERE ".trim($bedingung);
	else if(strlen($client_id))
	{
		$bedingung=" WHERE mandanten_id=".$client_id;
		if(($min_id == -1) && $client_id)
			$min_id=-1*($client_id*10000+100000);
	}
	else if(isset($GLOBALS['fxpglobals']['dbparam']['ctables'][$tnl]))
	{
		$bedingung=" WHERE mandanten_id";
		if(($tnl != 'felder') && ($tnl != 'masken'))
			$bedingung .= "=".$GLOBALS['fxpglobals']['client'];
		else
			$bedingung .= " IS NOT NULL";
	}

	// Spezialfall, wenn Werte nicht mittels Sequence ermittelt werden sollen
	if(!$min_id)
	{
		// Min-ID auf herkömmliche Weise holen
		$sql="SELECT MIN(".$fnl.") AS oldid FROM ".$tnl.$bedingung;
		$max_id=(int)db_value($sql);
		if(!$max_id)
			$max_id=-1;
		else
			$max_id--;

		return $max_id;
	}
	if(strlen($client_id))
		$client_id='m'.$client_id;

	// Sequencename generieren
	$seqname=fxf_dbSeqName($tnl, $fnl, $client_id, false);

	// Prüfen, ob Sequence vorhanden...
	$chkname=fxf_dbSeqCheck($seqname);
	if($chkname === false)
		return false;

	// ...ja, dann Wert mittels Sequence erniedrigen und prüfen
	if($chkname && $tabupdate)
	{
		$max_id=fxf_dbSeqValue($seqname, false);
		if($max_id !== false)
		{
			$bed=$bedingung;
			if(strlen($bed))
				$bed .= " AND";
			else
				$bed=" WHERE";
			$bed .= " ".$fnl."=".$max_id;

			$sql="SELECT ".$fnl." AS chk FROM ".$tnl.$bed;
			$chk=(int)db_value($sql);
		}
		else
			$chk=true;
		if($chk)
		{
			// Eine solche ID existiert bereits, deshalb Sequenz löschen
			$ierr=fxf_dbSeqDelete('', $tnl, $fnl, $client_id, false);
			if($ierr)
				return false;

			$chkname='';
		}
		else if(!isset($GLOBALS['db_mia'][$seqname]))
			$GLOBALS['db_mia'][$seqname]=$max_id+1;
	}

	// ...nein, dann Sequence erstellen und Startwert zurückliefern
	if(!$chkname || !$tabupdate)
	{
		// Startwert ermitteln
		$sql="SELECT MIN(".$fnl.") AS oldid FROM ".$tnl.$bedingung;
		$max_id=(int)db_value($sql);
		if(!$max_id)
			$max_id=$min_id;
		else
			$max_id=MIN($max_id-1, $min_id);

		if($tabupdate)
		{
			// Sequence erstellen
			$serr=fxf_dbSeqCreate($seqname, abs($max_id));
			if($serr)
				return false;


			$rmax_id=$max_id;
			$max_id=fxf_dbSeqValue($seqname, false);
			if($max_id === false)
			{
				$ierr=fxf_dbSeqDelete('', $tnl, $fnl, $client_id, false);
				if($ierr)
					return false;
				$max_id=$rmax_id;
			}
			else if(!isset($GLOBALS['db_mia'][$seqname]))
				$GLOBALS['db_mia'][$seqname]=$max_id+1;
		}
	}

	return $max_id;
}

/**
 * Create a sequence name from various parameters
 *
 * @param string  $tab_name   - Mandatory parameter:                  Name of the table
 * @param string  $field_name - Mandatory parameter:                  Name of the field
 * @param string  $client_id  - Optional parameter (default = ''):    ID of the client
 * @param boolean $maxname    - Optional parameter (default = true):  Sequence name for a max value (true) or a min value (false)
 *
 * @return Created sequence name
 */
function fxf_dbSeqName($tab_name, $field_name, $client_id='', $maxname=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Values
	$tnl=strtolower($tab_name);
	$fnl=strtolower($field_name);
	$client_id=trim($client_id);
	$sql_tab="SELECT id_tabellen FROM tabellen WHERE ".db_like('tabname', $tnl);
	$sql_fld="SELECT id, mandanten_id FROM felder WHERE ".db_like('feldname', $fnl)." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";

	// Generate sequence name
	// ...Prefix
	$seqname='seqm';
	if($maxname)
		$seqname .= 'x';
	else
		$seqname .= 'n';
	// ...Table
	$seqname .= 't';
	$tid=(int)db_value($sql_tab);
	if($tid < 0)
	{
		$seqname .= 'n';
		$tid=abs($tid);
	}
	$seqname .= $tid;
	// ...Field
	$seqname .= 'f';
	$far=db_values($sql_fld);
	if(!fxIsArray($far))
		$far=array('id'=>0, 'mandanten_id'=>0);
	$fid=(int)$far['id'];
	$mid=(int)$far['mandanten_id'];
	if($fid < 0)
	{
		$seqname .= 'n';
		$fid=abs($fid);
	}
	$seqname .= $fid;
	if($mid)
		$seqname .= 'm'.$mid;
	// ...Client
	$seqname .= $client_id;

	return $seqname;
}

/**
 * Create a table name from a sequence name
 *
 * @param string $seqname - Mandatory parameter:  Name of the sequence
 *
 * @return Created table name
 */
function fxf_dbSeqTableName($seqname)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$seqtabname='';
	if(strlen($seqname))
		$seqtabname=substr($seqname,0,3).'tab'.substr($seqname,3);

	return $seqtabname;
}

/**
 * Check if a sequence exists in the database
 *
 * @param string $seqname - Mandatory parameter:  Name of the sequence
 *
 * @return Found sequence name string (on succes), or false (on error)
 */
function fxf_dbSeqCheck($seqname)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Check if sequence exists
	$sql="";
	// ...PostgreSQL
	if($GLOBALS['_dbtype'] == 'pgsql')
		$sql="SELECT relname AS sname FROM pg_class WHERE relkind='S' AND LOWER(relname)='".$seqname."'";
	// ...SQLServer
	else if($GLOBALS['_dbtype'] == 'sqlsrv')
		$sql="SELECT name AS sname FROM sysobjects WHERE xtype='P' AND category=0 AND LOWER(name)='".$seqname."'";
	if(!strlen($sql))
		return false;
	$sname=db_value($sql);
	if(!$sname)
		$sname='';
	else if($GLOBALS['_dbtype'] == 'sqlsrv')
	{
		// Additional check for SQLServer if corresponding table exists
		$sql="SELECT id FROM sysobjects WHERE LOWER(name)='".fxf_dbSeqTableName($sname)."' AND type='U'";
		$stname=db_value($sql);
		if(!$stname)
			$sname='';
	}

	return $sname;
}

/**
 * Create a sequence (for PostgreSQL) or special table and procedure (for SQLServer) to handle our max and min id fields
 *
 * @param string  $seqname - Mandatory parameter:  Name of the sequence
 * @param integer $value  - Mandatory parameter:  ID start value
 *
 * @return false (on success), or true (on error)
 */
function fxf_dbSeqCreate($seqname, $value)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Create Sequence
	$sql="";
	// ...PostgreSQL
	if($GLOBALS['_dbtype'] == 'pgsql')
		$sql="CREATE SEQUENCE ".$seqname." START WITH ".$value." INCREMENT BY 1 NO MAXVALUE NO CYCLE";
	// ...SQLServer
	else if($GLOBALS['_dbtype'] == 'sqlsrv')
	{
		$seqtabname=fxf_dbSeqTableName($seqname);

		$sql="CREATE TABLE ".$seqtabname." (val INT NOT NULL IDENTITY(".$value.",1))";
		$suc=db_query($sql);
		if(!$suc)
			return true;

		$sql  = "CREATE PROC ".$seqname." @val AS INT OUTPUT".$GLOBALS['nl'];
		$sql .= "	AS".$GLOBALS['nl'];
		$sql .= "	BEGIN TRAN".$GLOBALS['nl'];
		$sql .= "		SAVE TRAN s1".$GLOBALS['nl'];
		$sql .= "		INSERT INTO ".$seqtabname." DEFAULT VALUES".$GLOBALS['nl'];
		$sql .= "		ROLLBACK TRAN s1".$GLOBALS['nl'];
		$sql .= "		SET @val=scope_identity()".$GLOBALS['nl'];
		$sql .= "	COMMIT TRAN";
	}
	if(!strlen($sql))
		return true;

	$suc=db_query($sql);
	if(!$suc)
		return true;
	return false;
}

/**
 * Get and return the next value in a sequence
 *
 * @param string  $seqname - Mandatory parameter:                  Name of the sequence
 * @param boolean $maxname - Optional parameter (default = true):  Max value (true) or Min value (false)
 *
 * @return Next integer value (on success), false (on error)
 */
function fxf_dbSeqValue($seqname, $maxname=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Get next value from sequence
	$sql="";
	// ...PostgreSQL
	if($GLOBALS['_dbtype'] == 'pgsql')
		$sql="SELECT nextval('".$seqname."') AS newid";
	// ...SQLServer
	else if($GLOBALS['_dbtype'] == 'sqlsrv')
		$sql="{CALL ".$seqname."(?)}";
	if(!strlen($sql))
		return false;

	$value=db_value($sql);
	if($value !== false)
	{
		$value=(int)$value;
		if(!$maxname)
			$value=-$value;
	}

	return $value;
}

/**
 * Reseed a squence value, i.e. set the identity or auto increment value
 *
 * @param string  $seqname - Mandatory parameter:               Name of the sequence
 * @param integer $value   - Optional parameter (default = 1):  New start value
 *
 * @return true (on success) or false (on error)
 */
function fxf_dbIdSeqReseed($seqname, $value=1)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Create reseed sql
	$sql="";
	if(strlen($seqname))
	{
		$value=(int)$value;
		// Check for the sequence
		$se=db_check($seqname, 's');
		if($se)
		{
			// PostgreSQL
			if($GLOBALS['_dbtype'] == 'pgsql')
				$sql="ALTER SEQUENCE ".$seqname." RESTART WITH ".$value;
			// SQLServer
			else if($GLOBALS['_dbtype'] == 'sqlsrv')
				$sql="DBCC CHECKIDENT ('".fxf_dbSeqTableName($seqname)."', RESEED, ".$value.") WITH NO_INFOMSGS";
		}
	}
	if(!strlen($sql))
		return false;

	// Execute sql and return
	$suc=@db_query($sql);
//fxDebug($suc,$sql, 0);

	if($suc)
		return true;
	return false;
}

/**
 * Delete a sequence (for PostgreSQL) or special table and procedure (for SQLServer)
 *
 * @param string  $pattern    - Optional parameter (default = ''):    Search pattern for the sequence to delete (if not submitted, the sequence name will be determined from the next parameters)
 * @param string  $tab_name   - Mandatory parameter:                  Name of the table
 * @param string  $field_name - Mandatory parameter:                  Name of the field
 * @param string  $client_id  - Optional parameter (default = ''):    ID of the client
 * @param boolean $maxname    - Optional parameter (default = true):  Sequence name for a max value (true) or a min value (false)
 * @param boolean $execute    - Optional parameter (default = true):  Execute the deletion (true) or return an array with all sql's
 *
 * @return true (on success) or false (on error) if the deletion should be executed, else an array with all sql's
 */
function fxf_dbSeqDelete($pattern='', $tab_name='', $field_name='', $client_id='', $maxname=true, $execute=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Werte zuweisen
	$sqlarr=array();
	$pattern=trim(strtolower($pattern));
	$tnl=strtolower($tab_name);
	$fnl=strtolower($field_name);

	if(strlen($pattern))
		$pattern='seqm%'.$pattern;
	else
		$pattern=fxf_dbSeqName($tnl, $fnl, $client_id, $maxname);

	// Check for the sequence
	$sql="";
	if($GLOBALS['_dbtype'] == 'sqlsrv')
		$sql="SELECT name AS sname FROM sysobjects WHERE xtype='P' AND category=0 AND LOWER(name) LIKE '".$pattern."'";
	else if($GLOBALS['_dbtype'] == 'pgsql')
		$sql="SELECT relname AS sname FROM pg_class WHERE relkind='S' AND LOWER(relname) LIKE '".$pattern."'";
	if(strlen($sql))
	{
		$seqnames=db_values($sql, '*');
		// Create deletion sql's for all found sequences
		if(fxIsArray($seqnames))
		{
			foreach($seqnames as $seq)
			{
				$seqname=$seq['sname'];
				if($GLOBALS['_dbtype'] == 'sqlsrv')
				{
					$sqlarr[]="DROP TABLE ".fxf_dbSeqTableName($seqname);
					$sqlarr[]="DROP PROC ".$seqname;
				}
				else
					$sqlarr[]="DROP SEQUENCE ".$seqname;
			}
		}
	}

	// No execution, return array with all sql's
	if(!$execute)
		return $sqlarr;

	// Execute all sql's
	if(sizeof($sqlarr))
	{
		foreach($sqlarr as $sql)
		{
			$suc=db_query($sql);
			if(!$suc)
				return true;
			else
			{
				$seqname='';
				$dc=substr($sql,5,3);
				if($dc == 'PRO')		// SQLServer's DROP PROC
					$seqname=trim(substr($sql,10));
				else if($dc == 'SEQ')	// PostgreSQL's DROP SEQUENCE
					$seqname=trim(substr($sql,14));
				if(strlen($seqname) && isset($GLOBALS['db_mia'][$seqname]))
					unset($GLOBALS['db_mia'][$seqname]);
			}
		}
	}

	return false;
}

/**
 * ???FUNCTION???
 *
 * @param boolean $intern  - Optional parameter (default = true): ???PARAMETER???
 * @param boolean $mitmand - Optional parameter (default = true): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_all_tables($intern=true, $mitmand=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql='';
	$tabarr=array();
	$dbytpe=$GLOBALS['_dbtypes'][$GLOBALS['fxpglobals']['dbparam']['dbtype']]['type'];

	// Alle intern gespeicherten Tabellen
	if($intern)
	{
		if($mitmand)
			$sql="SELECT t.tabname AS tn FROM tabellen t, tab_felder f WHERE t.id_tabellen=f.idintabellen AND f.idinfelder=22";
		else
			$sql="SELECT tabname AS tn FROM tabellen";
	}
	// Alle existierenden Tabellen
	else if($dbtype == 'sqlsrv')
		$sql="SELECT name AS tn FROM sysobjects WHERE type='U'";
	else if($dbtype == 'pgsql')
	{
		// Bei Postgres die Versionsnummer ermitteln wegen schemaname
		$where='';
		$pvn=db_value("SELECT VERSION()");
		if(strlen($pvn))
		{
			$dotpos1=strpos($pvn, '.');
			$vn1=(int)(substr($pvn, 11, $dotpos1));
			$dotpos2=strpos($pvn, '.', $dotpos1+1);
			$vn2=(int)(substr($pvn, $dotpos1+1, $dotpos2-$dotpos1-1));
			$vn3=(int)(substr($pvn, $dotpos2+1));
			if(($vn1 > 6) && ($vn2 > 3))	// Versionen ab 7.4.x können Schema
 				$where=" WHERE schemaname='public'";
 		}
		$sql="SELECT tablename AS tn FROM pg_tables".$where;
	}

	if($sql)
	{
		$tarr=db_values($sql, '*');
		if(fxIsArray($tarr))
		{
			foreach($tarr as $t)
				$tabarr[strtolower($t['tn'])]=$t['tn'];
		}
	}

//fxDebug($tabarr, 'Alle Tabellen');
	return $tabarr;
}

/**
 * Get header informations
 *
 * @param array $fa - Mandatory parameter: Array containing file lines
 *
 * @return Array with header informations
 */
function dbGetHeader($fa)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	////////////////////////////////////////////////////////////////////////////////
	// File name   : 021001000_vrs.txt                                            //
	// Version     : 21.1                                                         //
	// Versioncode : 021001000                                                    //
	// Date        : 2021-02-25                                                   //
	// Time        : 10:01:56                                                     //
	// Checksum    : b063d26a41ee72c676e2ecb0dc907d78                             //
	// ...                                                                        //
	$ha=array('filename'=>"", 'version'=>"", 'versioncode'=>"", 'date'=>"", 'time'=>"", 'checksum'=>"");
	if(fxIsArray($fa))
	{
		$sfa=min(8, sizeof($fa));
		foreach($ha as $hn => $dummy)
		{
			for($l=0; $l<$sfa; $l++)
			{
				if(strlen($fa[$l]) && (substr($fa[$l],0,3) == '// ') && (substr($fa[$l],15,1) == ':'))
				{
					$lit=strtolower(str_replace(' ', '', trim(substr($fa[$l],3,12))));
					if($lit == $hn)
					{
						$ha[$lit]=trim(substr($fa[$l],17,60));
						if($lit == 'filename')
							$ha[$lit]=trim(strtr($ha[$lit], array('_vrs'=>'', '.txt'=>'')));
						break;
					}
				}
			}
		}
	}

	return $ha;
}
?>