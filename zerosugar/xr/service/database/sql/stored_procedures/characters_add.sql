DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `characters_add`(
    IN account_id BIGINT,
    IN slot INT,
    IN name VARCHAR(20),
    IN level INT,
    IN str INT,
    IN dex INT,
    IN intell INT,
    IN job INT,
    IN face_id INT,
    IN hair_id INT,
    IN gold INT,
    IN zone_id INT,
    OUT character_id BIGINT
)
BEGIN
	INSERT INTO characters
		(aid, slot, name, level, str, dex, intell, job, face_id, hair_id, gold, zone_id)
	VALUES
		(account_id, slot, name, level, str, dex, intell, job, face_id, hair_id, gold, zone_id)
	;
    
    SET character_id = LAST_INSERT_ID();
END$$
DELIMITER ;
