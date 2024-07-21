CREATE TABLE `characters_equipment` (
  `cid` bigint(20) NOT NULL,
  `iid` bigint(20) NOT NULL,
  `equip_position` int(11) NOT NULL,
  KEY `iid_ce_fk_idx` (`iid`),
  KEY `cid_ce_fk_idx` (`cid`),
  CONSTRAINT `cid_ce_fk` FOREIGN KEY (`cid`) REFERENCES `characters` (`cid`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `iid_ce_fk` FOREIGN KEY (`iid`) REFERENCES `items` (`iid`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
