if($result = query("SELECT * FROM users"))
{
	if($count = $result->num_row)
	{
		$rows = $result->fetch_all(MYSQLI_ASSOC);
		foreach($rows as $row)
		{
			echo $row[];
		}
	}
}