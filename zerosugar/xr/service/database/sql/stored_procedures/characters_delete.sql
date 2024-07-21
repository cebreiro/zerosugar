DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `characters_delete`(
	IN character_id BIGINT
)
BEGIN
	UPDATE
		characters
	SET
		deleted = 1
	WHERE
		characters.cid = character_id;
END$$
DELIMITER ;
