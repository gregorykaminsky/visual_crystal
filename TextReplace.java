
import java.io.*;
import java.util.*;
import java.lang.*;
import java.lang.reflect.*;


public class TextReplace
{
	public static String replaceString(String input)
	{
		String[] extra = null;
		if( input.contains("//") == true)
		{
			extra = input.split("//");
			if(extra.length < 2)
				return input;
			
			input = extra[0];
		}
			
			
		String[] array = input.split(" ");
		for(int j = 0; j < array.length; j++)
		{
			if(array[j].equals("and"))
				array[j] = "&&";
			else if(array[j].equals("and\\"))
				array[j] = "&&\\";
			else if(array[j].equals("or"))
				array[j] = "||";
			else if(array[j].equals("or\\"))
				array[j] = "||";
		}
		
		if(extra == null)
			return String.join(" ", array);
		else
		{
			String output = String.join(" ", array);
			return output + "   //" + extra[1]; 
		}
	}
	
	
	
	public static void main(String[] args) throws Exception
	{
		File folder = new File(".");
		File[] listOfFiles = folder.listFiles();
		
	for (int i = 0; i < listOfFiles.length; i++) 
		if (listOfFiles[i].isFile() && 
		listOfFiles[i].getName().startsWith("ui_") == false &&
		(listOfFiles[i].getName().endsWith(".cpp") == true || listOfFiles[i].getName().endsWith(".h") == true))
		{ 
			System.out.println("Name of file:  " + listOfFiles[i].getName());
			Scanner in = new Scanner(listOfFiles[i]);
			ArrayList<String> list = new ArrayList<>();
			boolean comment = false;
			
			while(in.hasNextLine())
			{
				String line = in.nextLine();
				
				if(line.contains("/*") == true || line.contains("*/") == true)
				{
					if(line.contains("/*") == true)
					{
						String[] spliter = line.split("/\\*");
						if(spliter.length > 1)
						{
							spliter[0] = replaceString(spliter[0]);
							line = String.join("/*", spliter);
						}
						comment = true;
					}
					if(line.contains("*/") == true)
					{
						String[] spliter = line.split("\\*/");
						
						if(spliter.length > 1)
						{
							spliter[spliter.length-1] = replaceString(spliter[spliter.length-1]);
							line = String.join("*/", spliter);
						}
						comment = false;
					}
				}
				else if (comment == false)
				{
					line = replaceString(line);
				}
				list.add(line);
			}
			in.close();
			
			PrintWriter writer = new PrintWriter(listOfFiles[i].getName(), "UTF-8");
			for(int j = 0; j < list.size(); j++)
			{
				writer.println(list.get(j));
			}
			writer.close();
		
		}

	
	
	}

}
