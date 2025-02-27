

class LIMod  : GameSystem
{
	
	int gold = -1142236697;
	string password;
	string password2;
	string password3;
	[RplProp()]
	static string canfly;
	[RplProp()]
	static string canfly2;
	ref SCR_JsonLoadContext l;
	ref SCR_JsonSaveContext s;
	override protected void OnInit()
	{
		if(!Replication.IsServer())return;
		l = new 	SCR_JsonLoadContext(false);
		s = new SCR_JsonSaveContext();
		if(!l.LoadFromFile("Pizza.json"))
		{
			
			s.WriteValue("password","");
			s.WriteValue("password2","");
			s.WriteValue("password3","");
			s.WriteValue("Note"," Contact allaeddine_ on discord, All rights reserved to liberty interactive and spearhead");
			s.SaveToFile("Pizza.json");
		
		}
		
		l.ReadValue("password",password);
		l.ReadValue("password2",password2);
		l.ReadValue("password3",password3);
		feed( password,  password3,  password2);
	}
 		
	void feed(string apples, string poisen, string oranges)
	{
		canfly="no";
		canfly2 = "ni";
		int food = PizzaCook.CookInOven(apples+oranges);
		int rock = PizzaCook.CookInOven(apples+poisen +apples);
		if(gold == food)
		canfly = "yes";
		if(gold != rock)
		canfly2 = "da";
		if(canfly=="no"||canfly2=="ni" )
		Print("CANNOT USE MOD");
		
		if(canfly=="yes"&&canfly2=="da" )
		Print("CAN USE MOD");
		Replication.BumpMe();
//		GetGame().GetChat().GetDefaultChannel().ProcessMessage(GetGame().GetChat())
	}

}


class PizzaCook
{
    static int CookInOven(string input)
    {
        int hash = 0;
        
        for (int i = 0; i < input.Length(); i++)
        {
            hash += (int)input[i];
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        
        return hash;
    }
}
