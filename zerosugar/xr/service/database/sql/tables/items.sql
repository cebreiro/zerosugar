CREATE TABLE `items` (
  `iid` bigint(20) NOT NULL,
  `cid` bigint(20) NOT NULL,
  `data_id` int(11) NOT NULL,
  `quantity` int(11) NOT NULL,
  `slot` tinyint(4) DEFAULT NULL,
  `attack` int(11) DEFAULT NULL,
  `defence` int(11) DEFAULT NULL,
  `str` int(11) DEFAULT NULL,
  `dex` int(11) DEFAULT NULL,
  `intell` int(11) DEFAULT NULL,
  PRIMARY KEY (`iid`),
  KEY `cid_items_fk_idx` (`cid`),
  CONSTRAINT `cid_items_fk` FOREIGN KEY (`cid`) REFERENCES `characters` (`cid`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
