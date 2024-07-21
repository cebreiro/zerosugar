DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `lobby_characters_get_all`(
	account_id bigint
)
BEGIN
	SELECT 
		c.cid,
        c.slot,
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
		c.aid = account_id AND c.deleted = 0;
        
    SELECT
        i.cid,
        i.data_id,
        ce.equip_position
	FROM
		characters_equipment AS ce
	LEFT JOIN 
		items AS i
	ON
		ce.iid = i.iid
	WHERE 
		ce.cid IN (
        SELECT 
			characters.cid
		FROM
			characters
		WHERE
			characters.aid = account_id AND characters.deleted = 0
		);
END$$
DELIMITER ;
