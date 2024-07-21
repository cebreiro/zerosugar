CREATE TABLE `accounts` (
  `aid` bigint(20) NOT NULL AUTO_INCREMENT,
  `account` varchar(20) NOT NULL,
  `password` varchar(64) NOT NULL,
  `gm_level` tinyint(4) NOT NULL DEFAULT '0',
  `banned` tinyint(4) NOT NULL DEFAULT '0',
  `deleted` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`aid`),
  UNIQUE KEY `account_UNIQUE` (`account`)
) ENGINE=InnoDB AUTO_INCREMENT=120297 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
