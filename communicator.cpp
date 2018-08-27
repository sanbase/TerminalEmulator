/*
class TextUtil
{
private:
	boolean isWindows;

public:
	TextUtil()
	{
		char b=-120;
		String s=new String(b);
		if (((int)s.charAt(0))==136)
			isWindows=false;
		else isWindows=true;
	}
public:
	byte[] getBytes(String s)
	{
		byte b[];
		if (isWindows)
		{
			b=new byte[s.length()];
			for (int i=0; i<s.length(); i++)
			{
				if (((int)(s.charAt(i)))<128) b[i]=(byte)s.charAt(i);
				else b[i]=(byte)((int)s.charAt(i)-256);
			}
		}
		else
		{
			b=s.getBytes();
		}
		return b;
	}
}
*/
typedef BigInteger __int64

class RSAKey
{

private:
	BigInteger modulus;
	BigInteger exponent;

public:
	RSAKey(BigInteger in_modulus, BigInteger in_exponent)
	{
		modulus = in_modulus;
		exponent = in_exponent;
	}
	public BigInteger getModulus()
	{
		return modulus;
	}
	public BigInteger getExponent()
	{
		return exponent;
	}
}

public class RSA
{

	private:
		RSAKey key;
//	private TextUtil converter;

public:
	RSA(RSAKey in_key)
	{
		key=in_key;
		converter = new TextUtil();
	}

public:
	String encode(String text)
	{

		int tmp = key.getModulus().bitLength();
		int blockLength = key.getModulus().bitLength()/8 - 11;
		int actual = 0;
		int maxPos = text.length();
		byte[] byteok;
		BigInteger number, codednumber;
		String coded = new String();

		byteok = converter.getBytes(text);
		number = new BigInteger(byteok);
		byteok = doPadding(byteok);
		number= new BigInteger(byteok);
		codednumber = number.modPow(key.getExponent(), key.getModulus());
		coded = coded.concat(codednumber.toString(16));
		return coded;
	}


private:
	char * doPadding(byte D[])
	{
		int k = key.getModulus().bitLength()/8; //key length in octets
		int dataLength = D.length;              //datalength in octets
		int PSlength = k - 3 - dataLength;      //padding string length in octets
		byte PS[] = new byte[PSlength];         //padding string
		byte EB[] = new byte[k];                //encryption block

//fill PS with random and != 0
		Random random = new Random();
		long seed = System.currentTimeMillis();
		random.setSeed(seed);
		int nextRandom = 0;

		for (int i = 0; i < PSlength; i++)
		{
			while (nextRandom == 0)
			{
				nextRandom = random.nextInt() & 0x7f;
			}
			PS[i] = (byte)nextRandom;
			nextRandom = 0;
		}

		EB[0] = 0;
		EB[1] = 2;
		for (int i = 0; i < PSlength; i++)
		{
			EB[i+2] = PS[i];
		}
		EB[PSlength+2] = 0;
		for (int i = 0; i < dataLength; i++)
		{
			EB[PSlength+3+i] = D[i];
		}
		return EB;
	}

	public void setKey(RSAKey in_key)
	{
		key=in_key;
	}
}

public class Communicator
{

	private byte desc = (byte)0x83;
	private byte[] sessionKey = new byte[2];

	private RSA rsa;
	private             BigInteger modulus;
	private             BigInteger exponent;

	private RSAKey publicKey = null;

	public Communicator(String pubkey)
	{
		try
		{
			publicKey = doMakePublicKey(pubkey);
			rsa = new RSA(publicKey);
		}
		catch(Exception e)
		{
			System.out.println("Exception in Communicator():"+e.toString());
		}
	}

	public byte[] doSendRSAMessage(String messageToSend)
	{
		try
		{
			byte sendMessageBytes[];
			String encoded = rsa.encode(messageToSend);
			BigInteger encodedBI = new BigInteger(encoded, 16);
			sendMessageBytes = encodedBI.toByteArray();

			if (sendMessageBytes[0] == 0)
			{
				byte tmpBytes[] = new byte[64];
				for (int i = 0;  i < 64; i++)
				{
					tmpBytes[i] = sendMessageBytes[i+1];
				}
				sendMessageBytes = tmpBytes;
			}
			return sendMessageBytes;
		}
		catch(Exception e)
		{
			System.out.println("Exception in doSendRSAMessage:"+e.toString());
			return null;
		}
	}

	private RSAKey doMakePublicKey(String pubkey)
	{
		try
		{
			BigInteger modulus = pubkey;
			BigInteger exponent = 65537;
			return new RSAKey(modulus, exponent);
		}
		catch(Exception e)
		{
			System.out.println("Exception in doMakePublicKey()");
			return null;
		}
	}
}
