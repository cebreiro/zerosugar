DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `characters_get`(
	IN character_id BIGINT
)
BEGIN
SELECT 
		c.cid,
        c.name,
        c.level,
        c.str,
        c.dex,
        c.intell,
        c.job,
        c.face_id,
        c.hair_id,
        c.zone_id
    FROM
		characters AS c
	WHERE
		c.cid = character_id;
        
    SELECT
		i.iid,
        i.data_id,
        i.quantity,
        i.slot,
        i.attack,
        i.defence,
        i.str,
        i.dex,
        i.intell
	FROM
		items AS i
	WHERE 
		i.cid = character_id;
        
    SELECT
        ce.iid,
        ce.equip_position
	FROM
		characters_equipment AS ce
	WHERE 
		ce.cid = character_id;
END$$
DELIMITER ;
