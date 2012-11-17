# === configuration :
IP, PORT = '192.168.1.2', 8761

# === plans de tests :
JEUX_DE_TESTS = [

	{
		'titre': 'ok',
		'commandes': [
			'I#porteSlips,4000,1,1,1,1,1,1',
			'C#porteSlips,300'
		],
		'retour': [],
	},

	{
		'titre': 'nok',
		'commandes': [
			'LOL!'
		],
		'retour': [],
	},

]