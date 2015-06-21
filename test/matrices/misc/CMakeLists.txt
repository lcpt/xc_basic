# Archivos fuente.
SET(SOURCES1 xa_cajas xmatriz xmatriz1 xmatriz2 xmatriz4 xmatsim xvcolM xmatdisp xmatrizT xop_tensor xmatriz0 xmatsim0 xTrn string_to_matriz )

SET(SOURCES2 xmatriz3 xMatrizM xmatrizM1 string_to_matrizExpr m_double_to_matrizExpr)

# Creamos ejecutables
FOREACH(source ${SOURCES1})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} Matrices)
ENDFOREACH(source)

FOREACH(source ${SOURCES2})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} lcmd Matrices funciones Texto Tiempo boost_regex)
ENDFOREACH(source)




