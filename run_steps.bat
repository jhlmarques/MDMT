@echo off
echo GERAÇÃO POR PASSOS (SEM ASPIRAÇÃO)
cd build\Release
.\tabusearch ..\..\testes_passo 800 10 0 1 0 < ..\..\mdmt39.112.A.ins
											
.\tabusearch ..\..\testes_passo 800 30 0 1 0 < ..\..\mdmt39.112.A.ins
									   
.\tabusearch ..\..\testes_passo 800 50 0 1 0 < ..\..\mdmt39.112.A.ins
									   
.\tabusearch ..\..\testes_passo 800 70 0 1 0 < ..\..\mdmt39.112.A.ins
											
.\tabusearch ..\..\testes_passo 800 90 0 1 0 < ..\..\mdmt39.112.A.ins
									   
echo GERAÇÃO POR PASSOS (COM ASPIRAÇÃO)0
.\tabusearch ..\..\testes_passo 800 10 0 1 1 < ..\..\mdmt39.112.A.ins
									   
.\tabusearch ..\..\testes_passo 800 30 0 1 1 < ..\..\mdmt39.112.A.ins
									   
.\tabusearch ..\..\testes_passo 800 50 0 1 1 < ..\..\mdmt39.112.A.ins
									   
.\tabusearch ..\..\testes_passo 800 70 0 1 1 < ..\..\mdmt39.112.A.ins
									   
.\tabusearch ..\..\testes_passo 800 90 0 1 1 < ..\..\mdmt39.112.A.ins