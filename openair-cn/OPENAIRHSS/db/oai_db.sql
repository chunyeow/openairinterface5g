-- phpMyAdmin SQL Dump
-- version 3.4.10.1deb1
-- http://www.phpmyadmin.net
--
-- Client: localhost
-- GÃ©nÃ©rÃ© le : Mer 18 DÃ©cembre 2013 Ã  18:01
-- Version du serveur: 5.5.34
-- Version de PHP: 5.3.10-1ubuntu3.9

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Base de donnÃ©es: `oai_db`
--

-- --------------------------------------------------------

--
-- Structure de la table `apn`
--

CREATE TABLE IF NOT EXISTS `apn` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `apn-name` varchar(60) NOT NULL,
  `pdn-type` enum('IPv4','IPv6','IPv4v6','IPv4_or_IPv6') NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `apn-name` (`apn-name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Structure de la table `mmeidentity`
--

CREATE TABLE IF NOT EXISTS `mmeidentity` (
  `idmmeidentity` int(11) NOT NULL AUTO_INCREMENT,
  `mmehost` varchar(255) DEFAULT NULL,
  `mmerealm` varchar(200) DEFAULT NULL,
  `UE-Reachability` tinyint(1) NOT NULL COMMENT 'Indicates whether the MME supports UE Reachability Notifcation',
  PRIMARY KEY (`idmmeidentity`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=39 ;

--
-- Contenu de la table `mmeidentity`
--

INSERT INTO `mmeidentity` (`idmmeidentity`, `mmehost`, `mmerealm`, `UE-Reachability`) VALUES
(29, 'yang.eur', 'eur', 0),
(28, 'nord.eur', 'eur', 0),
(30, 'hades.eur', 'eur', 0),
(35, 'olympie-Latitude-E6520.eur', 'eur', 0),
(33, 'orcus.eur', 'eur', 0),
(36, 'caviar.eur', 'eur', 0),
(37, 'sud.eur', 'eur', 0),
(38, 'tapenade.eur', 'eur', 1),
(39, 'mme0.eur', 'eur', 0);

-- --------------------------------------------------------

--
-- Structure de la table `pdn`
--

CREATE TABLE IF NOT EXISTS `pdn` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `apn` varchar(60) NOT NULL,
  `pdn_type` enum('IPv4','IPv6','IPv4v6','IPv4_or_IPv6') NOT NULL DEFAULT 'IPv4',
  `pdn_ipv4` varchar(15) DEFAULT '0.0.0.0',
  `pdn_ipv6` varchar(45) CHARACTER SET latin1 COLLATE latin1_general_ci DEFAULT '0:0:0:0:0:0:0:0',
  `aggregate_ambr_ul` int(10) unsigned DEFAULT '50000000',
  `aggregate_ambr_dl` int(10) unsigned DEFAULT '100000000',
  `pgw_id` int(11) NOT NULL,
  `users_imsi` varchar(15) NOT NULL,
  `qci` tinyint(3) unsigned NOT NULL DEFAULT '9',
  `priority_level` tinyint(3) unsigned NOT NULL DEFAULT '15',
  `pre_emp_cap` enum('ENABLED','DISABLED') DEFAULT 'DISABLED',
  `pre_emp_vul` enum('ENABLED','DISABLED') DEFAULT 'DISABLED',
  `LIPA-Permissions` enum('LIPA-prohibited','LIPA-only','LIPA-conditional') NOT NULL DEFAULT 'LIPA-only',
  PRIMARY KEY (`id`,`pgw_id`,`users_imsi`),
  KEY `fk_pdn_pgw1_idx` (`pgw_id`),
  KEY `fk_pdn_users1_idx` (`users_imsi`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=8 ;

--
-- Contenu de la table `pdn`
--

INSERT INTO `pdn` (`id`, `apn`, `pdn_type`, `pdn_ipv4`, `pdn_ipv6`, `aggregate_ambr_ul`, `aggregate_ambr_dl`, `pgw_id`, `users_imsi`, `qci`, `priority_level`, `pre_emp_cap`, `pre_emp_vul`, `LIPA-Permissions`) VALUES
(2, 'wap.test.fr', 'IPv4v6', '10.0.0.5', '0:0:0:0:0:0:0:1', 1048576, 14400000, 2, '20834123456789', 8, 2, 'ENABLED', 'DISABLED', 'LIPA-only'),
(3, 'toto.eurecom.fr', 'IPv4v6', '0.0.0.0', '2001:0db8:85a3:0042:1000:8a2e:0370:7334', 50000, 100000, 2, '20834123456710', 1, 3, 'DISABLED', 'DISABLED', 'LIPA-only'),
(4, 'edge.eurecom.fr', 'IPv4_or_IPv6', '0.0.0.0', '2001:0db8:85a3:0042:1000:8a2e:0370:7356', 50000000, 100000000, 1, '20834123456789', 3, 4, 'ENABLED', 'DISABLED', 'LIPA-only'),
(7, 'internet.v6.eur', 'IPv6', '0.0.0.0', '0:0:0:0:0:0:0:0', 50000000, 100000000, 4, '20834123456789', 9, 15, 'DISABLED', 'ENABLED', 'LIPA-only'),
(1, 'internet.v4.eur', 'IPv4', '0.0.0.0', '0:0:0:0:0:0:0:0', 50000000, 100000000, 3, '20834123456789', 9, 15, 'DISABLED', 'ENABLED', 'LIPA-only'),
(8, 'internet.v4.eur', 'IPv4', '0.0.0.0', '0:0:0:0:0:0:0:0', 50000000, 100000000, 3, '208920000000008', 9, 15, 'DISABLED', 'ENABLED', 'LIPA-only'),
(9, 'internet.v4.eur', 'IPv4', '0.0.0.0', '0:0:0:0:0:0:0:0', 50000000, 100000000, 3, '208920000000009', 9, 15, 'DISABLED', 'ENABLED', 'LIPA-only'),
(10, 'internet.v4.eur', 'IPv4', '0.0.0.0', '0:0:0:0:0:0:0:0', 50000000, 100000000, 3, '20810000001234', 9, 15, 'DISABLED', 'ENABLED', 'LIPA-only');
-- --------------------------------------------------------

--
-- Structure de la table `pgw`
--

CREATE TABLE IF NOT EXISTS `pgw` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `ipv4` varchar(15) NOT NULL,
  `ipv6` varchar(39) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `ipv4` (`ipv4`),
  UNIQUE KEY `ipv6` (`ipv6`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=4 ;

--
-- Contenu de la table `pgw`
--

INSERT INTO `pgw` (`id`, `ipv4`, `ipv6`) VALUES
(1, '127.0.0.1', '0:0:0:0:0:0:0:1'),
(2, '192.168.56.101', ''),
(3, '10.0.0.2', '0');

-- --------------------------------------------------------

--
-- Structure de la table `terminal-info`
--

CREATE TABLE IF NOT EXISTS `terminal-info` (
  `imei` varchar(15) NOT NULL,
  `sv` varchar(2) NOT NULL,
  UNIQUE KEY `imei` (`imei`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Structure de la table `users`
--

CREATE TABLE IF NOT EXISTS `users` (
  `imsi` varchar(15) NOT NULL COMMENT 'IMSI is the main reference key.',
  `msisdn` varchar(46) DEFAULT NULL COMMENT 'The basic MSISDN of the UE (Presence of MSISDN is optional).',
  `imei` varchar(15) DEFAULT NULL COMMENT 'International Mobile Equipment Identity',
  `imei_sv` varchar(2) DEFAULT NULL COMMENT 'International Mobile Equipment Identity Software Version Number',
  `ms_ps_status` enum('PURGED','NOT_PURGED') DEFAULT 'PURGED' COMMENT 'Indicates that ESM and EMM status are purged from MME',
  `rau_tau_timer` int(10) unsigned DEFAULT '120' COMMENT '',
  `ue_ambr_ul` bigint(20) unsigned DEFAULT '50000000' COMMENT 'The Maximum Aggregated uplink MBRs to be shared across all Non-GBR bearers according to the subscription of the user.',
  `ue_ambr_dl` bigint(20) unsigned DEFAULT '100000000' COMMENT 'The Maximum Aggregated downlink MBRs to be shared across all Non-GBR bearers according to the subscription of the user.',
  `access_restriction` int(10) unsigned DEFAULT '60' COMMENT 'Indicates the access restriction subscription information. 3GPP TS.29272 #7.3.31',
  `mme_cap` int(10) unsigned zerofill DEFAULT NULL COMMENT 'Indicates the capabilities of the MME with respect to core functionality e.g. regional access restrictions.',
  `mmeidentity_idmmeidentity` int(11) NOT NULL DEFAULT '0' COMMENT '',
  `key` varbinary(16) NOT NULL DEFAULT '0' COMMENT 'UE security key',
  `RFSP-Index` smallint(5) unsigned NOT NULL DEFAULT '1' COMMENT 'An index to specific RRM configuration in the E-UTRAN. Possible values from 1 to 256',
  `urrp_mme` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'UE Reachability Request Parameter indicating that UE activity notification from MME has been requested by the HSS.',
  `sqn` bigint(20) unsigned zerofill NOT NULL COMMENT '',
  `rand` varbinary(16) NOT NULL COMMENT '',
  PRIMARY KEY (`imsi`,`mmeidentity_idmmeidentity`),
  KEY `fk_users_mmeidentity_idx1` (`mmeidentity_idmmeidentity`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Contenu de la table `users`
--

INSERT INTO `users` (`imsi`, `msisdn`, `imei`, `imei_sv`, `ms_ps_status`, `rau_tau_timer`, `ue_ambr_ul`, `ue_ambr_dl`, `access_restriction`, `mme_cap`, `mmeidentity_idmmeidentity`, `key`, `RFSP-Index`, `urrp_mme`, `sqn`, `rand`) VALUES
('20834123456789', '380561234567', '12345678', '23', 'PURGED', 50, 50000000, 100000000, 47, 0000000000, 36, '+ÖEŸ‚Å³\0•,IHÿH', 0, 0, 0000000096, 'Px¼X \Z1¡Éx™ß'),
('208920000000008', '33638060008', NULL, NULL, 'PURGED', 120, 50000000, 100000000, 60, 0000000000, 0, '‹¯G?/Ð”‡ÌË×	|hb', 1, 0, 0000027276, '''©$Ìy2¨dËêmù'),
('208920000000009', '33638060009', NULL, NULL, 'PURGED', 120, 50000000, 100000000, 47, 0000000000, 0, '‹¯G?/Ð”WdiHC×¾½', 1, 0, 0000027276, '\r¨`ËdŽ¶_íŽÃdÀ'),
('20810000001234', '33611123456', NULL, NULL, 'PURGED', 120, 50000000, 100000000, 47, 0000000000, 39, '‹¯G?/Ð”‡ÌË×	|hb', 1, 0, 0000009600, 'LyV€³L¾uz†Ã>m¯¹');

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
